// Header
#include "world_system.hpp"

#include "world_init.hpp"

// stlib
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

#include "physics_system.hpp"
#include "window_manager.hpp"

#define PI 3.14159265

// Game configuration

ConstrainedPhysicsWorldSystem::ConstrainedPhysicsWorldSystem() {
  // Seeding rng with random device
  rng = std::default_random_engine(std::random_device()());
}

ConstrainedPhysicsWorldSystem::~ConstrainedPhysicsWorldSystem() {
  // Destroy all created components
  registry->clear_all_components();

  // clean up all the shared pointers
  renderer = nullptr;
  registry = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void ConstrainedPhysicsWorldSystem::init(
    std::shared_ptr<ConstrainedPhysicsRegistry> registry,
    std::shared_ptr<RenderSystem> renderer,
    std::shared_ptr<ConstrainedPhysicsSystem> physics,
    std::shared_ptr<WindowManager> window_manager,
    std::function<void()> on_scene_end) {
  this->registry = registry;
  this->renderer = renderer;
  this->physics = physics;
  this->window_manager = window_manager;
  this->on_game_end_callback_ptr = on_scene_end;

  // Set all states to default
  restart_game();
}

void ConstrainedPhysicsWorldSystem::recalculateAngle(Entity e, vec2 a, vec2 b) {
  float diffY = b.y - a.y;
  float diffX = b.x - a.x;
  registry->ropes.get(e).angle = atan2(diffY, diffX);
  TransformComponent& transform = registry->transforms.get(e);
  transform.position = {(a.x + b.x) / 2, (a.y + b.y) / 2};
  transform.scale = {sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)), 10};
}

void ConstrainedPhysicsWorldSystem::handleVerticalSpring() {
  float L0 = 1;      // natural rope length
  float ks = 100.f;  // spring stiffness
  float mass = 1.f;
  float grav = 0.1f;
  (void)grav;

  TransformComponent topBallTransform = registry->transforms.get(topBallVert);
  TransformComponent middleBallTransform =
      registry->transforms.get(middleBallVert);
  TransformComponent bottomBallTransform =
      registry->transforms.get(bottomBallVert);

  vec2 L = middleBallTransform.position - topBallTransform.position;
  float magL = sqrt((L.x * L.x) + (L.y * L.y));
  vec2 lHat = L / magL;  // calculate spring direction
  float s = magL - L0;   // amount spring has been stretched/compressed
  vec2 F_spring = -ks * s * lHat;  // calculate spring force

  vec2 L2 = bottomBallTransform.position - middleBallTransform.position;
  float magL2 = sqrt((L2.x * L2.x) + (L2.y * L2.y));
  vec2 lHat2 = L2 / magL2;  // calculate spring direction
  float s2 = magL2 - L0;    // amount spring has been stretched/compressed
  vec2 F_spring2 = -ks * s2 * lHat2;  // calculate spring force

  vec2 gravVec = {1, 1};          // {0,1}
  vec2 F_grav = -mass * gravVec;  //* grav * gravVec;

  // energy =

  vec2 force = F_spring + F_grav - F_spring2;  // total force on middleBall
  vec2 force2 = F_spring2 + F_grav;            // total force on bottomBall
  float delta = 0.001f;
  Velocity& middleVel = registry->velocities.get(middleBallVert);
  middleVel.velocity += force / mass * delta;
  middleBallTransform.position += middleVel.velocity * delta;
  Velocity& bottomVel = registry->velocities.get(bottomBallVert);
  bottomVel.velocity += force2 / mass * delta;
  bottomBallTransform.position += bottomVel.velocity * delta;
  TransformComponent& topMiddleLinkTransform =
      registry->transforms.get(topMiddleLinkVert);
  (void)topMiddleLinkTransform;
  ConstrainedPhysicsRegistry::Rope& topMiddleRope =
      registry->ropes.get(topMiddleLinkVert);
  topMiddleRope.a = topBallTransform.position;
  topMiddleRope.b = middleBallTransform.position;
  recalculateAngle(topMiddleLinkVert, topMiddleRope.a, topMiddleRope.b);
  ConstrainedPhysicsRegistry::Rope& middleBottomRope =
      registry->ropes.get(middleBottomLinkVert);
  middleBottomRope.a = middleBallTransform.position;
  middleBottomRope.b = bottomBallTransform.position;
  recalculateAngle(middleBottomLinkVert, middleBottomRope.a,
                   middleBottomRope.b);
}

void ConstrainedPhysicsWorldSystem::handleHorizontalSpring() {
  float L0 = 1;      // natural rope length
  float ks = 115.f;  // spring stiffness
  float mass = 1.f;
  float grav = 0.1f;
  (void)grav;

  TransformComponent topBallTransform = registry->transforms.get(topBallHorz);
  TransformComponent middleBallTransform =
      registry->transforms.get(middleBallHorz);
  TransformComponent bottomBallTransform =
      registry->transforms.get(bottomBallHorz);

  vec2 L = middleBallTransform.position - topBallTransform.position;
  float magL = sqrt((L.x * L.x) + (L.y * L.y));
  vec2 lHat = L / magL;  // calculate spring direction
  float s = magL - L0;   // amount spring has been stretched/compressed
  vec2 F_spring = -ks * s * lHat;  // calculate spring force

  vec2 L2 = bottomBallTransform.position - middleBallTransform.position;
  float magL2 = sqrt((L2.x * L2.x) + (L2.y * L2.y));
  vec2 lHat2 = L2 / magL2;  // calculate spring direction
  float s2 = magL2 - L0;    // amount spring has been stretched/compressed
  vec2 F_spring2 = -ks * s2 * lHat2;  // calculate spring force

  vec2 gravVec = {1, 1};          // {0,1}
  vec2 F_grav = -mass * gravVec;  //* grav * gravVec;

  vec2 force = F_spring + F_grav - F_spring2;  // total force on middleBall
  vec2 force2 = F_spring2 + F_grav;            // total force on bottomBall
  float delta = 0.001f;
  Velocity& middleVel = registry->velocities.get(middleBallHorz);
  middleVel.velocity += force / mass * delta;
  middleBallTransform.position += middleVel.velocity * delta;
  Velocity& bottomVel = registry->velocities.get(bottomBallHorz);
  bottomVel.velocity += force2 / mass * delta;
  bottomBallTransform.position += bottomVel.velocity * delta;
  TransformComponent& topMiddleLinkTransform =
      registry->transforms.get(topMiddleLinkHorz);
  (void)topMiddleLinkTransform;
  ConstrainedPhysicsRegistry::Rope& topMiddleRope =
      registry->ropes.get(topMiddleLinkHorz);
  topMiddleRope.a = topBallTransform.position;
  topMiddleRope.b = middleBallTransform.position;
  recalculateAngle(topMiddleLinkHorz, topMiddleRope.a, topMiddleRope.b);
  ConstrainedPhysicsRegistry::Rope& middleBottomRope =
      registry->ropes.get(middleBottomLinkHorz);
  middleBottomRope.a = middleBallTransform.position;
  middleBottomRope.b = bottomBallTransform.position;
  recalculateAngle(middleBottomLinkHorz, middleBottomRope.a,
                   middleBottomRope.b);
}

void ConstrainedPhysicsWorldSystem::handleDiagSpring1() {
  float L0 = 1;      // natural rope length
  float ks = 130.f;  // spring stiffness
  float mass = 1.f;
  float grav = 0.1f;
  (void)grav;

  TransformComponent topBallTransform = registry->transforms.get(topBallDiag1);
  TransformComponent middleBallTransform =
      registry->transforms.get(middleBallDiag1);
  TransformComponent bottomBallTransform =
      registry->transforms.get(bottomBallDiag1);

  vec2 L = middleBallTransform.position - topBallTransform.position;
  float magL = sqrt((L.x * L.x) + (L.y * L.y));
  vec2 lHat = L / magL;  // calculate spring direction
  float s = magL - L0;   // amount spring has been stretched/compressed
  vec2 F_spring = -ks * s * lHat;  // calculate spring force

  vec2 L2 = bottomBallTransform.position - middleBallTransform.position;
  float magL2 = sqrt((L2.x * L2.x) + (L2.y * L2.y));
  vec2 lHat2 = L2 / magL2;  // calculate spring direction
  float s2 = magL2 - L0;    // amount spring has been stretched/compressed
  vec2 F_spring2 = -ks * s2 * lHat2;  // calculate spring force

  vec2 gravVec = {1, 1};          // {0,1}
  vec2 F_grav = -mass * gravVec;  //* grav * gravVec;

  vec2 force = F_spring + F_grav - F_spring2;  // total force on middleBall
  vec2 force2 = F_spring2 + F_grav;            // total force on bottomBall
  float delta = 0.001f;
  Velocity& middleVel = registry->velocities.get(middleBallDiag1);
  middleVel.velocity += force / mass * delta;
  middleBallTransform.position += middleVel.velocity * delta;
  Velocity& bottomVel = registry->velocities.get(bottomBallDiag1);
  bottomVel.velocity += force2 / mass * delta;
  bottomBallTransform.position += bottomVel.velocity * delta;
  TransformComponent& topMiddleLinkTransform =
      registry->transforms.get(topMiddleLinkDiag1);
  (void)topMiddleLinkTransform;
  ConstrainedPhysicsRegistry::Rope& topMiddleRope =
      registry->ropes.get(topMiddleLinkDiag1);
  topMiddleRope.a = topBallTransform.position;
  topMiddleRope.b = middleBallTransform.position;
  recalculateAngle(topMiddleLinkDiag1, topMiddleRope.a, topMiddleRope.b);
  ConstrainedPhysicsRegistry::Rope& middleBottomRope =
      registry->ropes.get(middleBottomLinkDiag1);
  middleBottomRope.a = middleBallTransform.position;
  middleBottomRope.b = bottomBallTransform.position;
  recalculateAngle(middleBottomLinkDiag1, middleBottomRope.a,
                   middleBottomRope.b);
}

void ConstrainedPhysicsWorldSystem::handleDiagSpring2() {
  float L0 = 1;      // natural rope length
  float ks = 145.f;  // spring stiffness
  float mass = 1.f;
  float grav = 0.1f;
  (void)grav;

  TransformComponent topBallTransform = registry->transforms.get(topBallDiag2);
  TransformComponent middleBallTransform =
      registry->transforms.get(middleBallDiag2);
  TransformComponent bottomBallTransform =
      registry->transforms.get(bottomBallDiag2);

  vec2 L = middleBallTransform.position - topBallTransform.position;
  float magL = sqrt((L.x * L.x) + (L.y * L.y));
  vec2 lHat = L / magL;  // calculate spring direction
  float s = magL - L0;   // amount spring has been stretched/compressed
  vec2 F_spring = -ks * s * lHat;  // calculate spring force

  vec2 L2 = bottomBallTransform.position - middleBallTransform.position;
  float magL2 = sqrt((L2.x * L2.x) + (L2.y * L2.y));
  vec2 lHat2 = L2 / magL2;  // calculate spring direction
  float s2 = magL2 - L0;    // amount spring has been stretched/compressed
  vec2 F_spring2 = -ks * s2 * lHat2;  // calculate spring force

  vec2 gravVec = {1, 1};          // {0,1}
  vec2 F_grav = -mass * gravVec;  //* grav * gravVec;

  vec2 force = F_spring + F_grav - F_spring2;  // total force on middleBall
  vec2 force2 = F_spring2 + F_grav;            // total force on bottomBall
  float delta = 0.001f;
  Velocity& middleVel = registry->velocities.get(middleBallDiag2);
  middleVel.velocity += force / mass * delta;
  middleBallTransform.position += middleVel.velocity * delta;
  Velocity& bottomVel = registry->velocities.get(bottomBallDiag2);
  bottomVel.velocity += force2 / mass * delta;
  bottomBallTransform.position += bottomVel.velocity * delta;
  TransformComponent& topMiddleLinkTransform =
      registry->transforms.get(topMiddleLinkDiag2);
  (void)topMiddleLinkTransform;
  ConstrainedPhysicsRegistry::Rope& topMiddleRope =
      registry->ropes.get(topMiddleLinkDiag2);
  topMiddleRope.a = topBallTransform.position;
  topMiddleRope.b = middleBallTransform.position;
  recalculateAngle(topMiddleLinkDiag2, topMiddleRope.a, topMiddleRope.b);
  ConstrainedPhysicsRegistry::Rope& middleBottomRope =
      registry->ropes.get(middleBottomLinkDiag2);
  middleBottomRope.a = middleBallTransform.position;
  middleBottomRope.b = bottomBallTransform.position;
  recalculateAngle(middleBottomLinkDiag2, middleBottomRope.a,
                   middleBottomRope.b);
}

// Update our game world
bool ConstrainedPhysicsWorldSystem::step(float delta) {
  // update camera
  // registry->camera.get(camera).cameraTarget =
  //    registry->transforms.get(current_player).position;
  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  // check if player has won
  auto playerTransform = registry->transforms.get(current_player);
  if (playerTransform.position.x > screen_width && !wonYet) {
    wonYet = true;
    registry->velocities.get(current_player).velocity = {0, 0};
    registry->deathTimers.emplace(current_player);
    //registry->colors.get(current_player).r = 0;
    registry->colors.get(current_player).g = 255;
    //registry->colors.get(current_player).b = 0;
  }

  // add step functions here
  handleVerticalSpring();
  handleHorizontalSpring();
  handleDiagSpring1();
  handleDiagSpring2();

  float min_counter_ms = 3000.f;
  for (Entity entity : registry->deathTimers.entities) {
    // progress timer
    DeathTimer& counter = registry->deathTimers.get(entity);
    counter.counter_ms -= delta;
    if (counter.counter_ms < min_counter_ms) {
      min_counter_ms = counter.counter_ms;
    }

    // restart the game once the death timer expired
    if (counter.counter_ms < 0) {
      registry->deathTimers.remove(entity);
      // restart_game();
      on_game_end_callback_ptr();
      return true;
    }
  }

  handle_collisions();

  return true;
}

// Reset the world state to its initial state
void ConstrainedPhysicsWorldSystem::restart_game() {
  wonYet = false;
  deadYet = false;
  if (!registry->camera.has(camera)) {
    camera = Entity();
    registry->camera.emplace(camera);
  }
  // Debugging for memory/component leaks
  registry->list_all_components();
  printf("Restarting\n");

  // Remove all entities that we created
  // All that have a motion, we could also iterate over all fish, turtles, ...
  // but that would be more cumbersome
  while (registry->transforms.entities.size() > 0)
    registry->remove_all_components_of(registry->transforms.entities.back());

  // Debugging for memory/component leaks
  registry->list_all_components();

  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  createBackground(registry, renderer, {screen_width/2, screen_height/2});

  current_player =
      createPlayer(registry, renderer, {screen_width / 8, screen_height / 3});

  registry->camera.get(camera).cameraPosition = {screen_width / 2,
                                                 screen_height / 2};

  registry->camera.get(camera).cameraTarget = {screen_width / 2,
                                               screen_height / 2};

  registry->camera.get(camera).cameraFOV = {screen_width, screen_height};

  // vertical spring
  topBallVert =
      createLine(registry, {screen_width / 2, screen_height / 2}, {30, 30});
  middleBallVert =
      createLine(registry, {screen_width / 2, screen_height * 3 / 4}, {30, 30});
  topMiddleLinkVert =
      createRope(registry, registry->transforms.get(topBallVert).position,
                 registry->transforms.get(middleBallVert).position);
  bottomBallVert =
      createLine(registry, {screen_width / 2, screen_height}, {30, 30});
  middleBottomLinkVert =
      createRope(registry, registry->transforms.get(middleBallVert).position,
                 registry->transforms.get(bottomBallVert).position);

  // horizontal spring
  topBallHorz =
      createLine(registry, {screen_width / 2, screen_height / 2}, {30, 30});
  middleBallHorz =
      createLine(registry, {screen_width * 3 / 4, screen_height / 2}, {30, 30});
  topMiddleLinkHorz =
      createRope(registry, registry->transforms.get(topBallHorz).position,
                 registry->transforms.get(middleBallHorz).position);
  bottomBallHorz =
      createLine(registry, {screen_width, screen_height / 2}, {30, 30});
  middleBottomLinkHorz =
      createRope(registry, registry->transforms.get(middleBallHorz).position,
                 registry->transforms.get(bottomBallHorz).position);

  // diagonal spring 1
  topBallDiag1 =
      createLine(registry, {screen_width / 2, screen_height / 2}, {30, 30});
  middleBallDiag1 = createLine(
      registry, {screen_width * 3 / 4, screen_height * 3 / 4}, {30, 30});
  topMiddleLinkDiag1 =
      createRope(registry, registry->transforms.get(topBallDiag1).position,
                 registry->transforms.get(middleBallDiag1).position);
  bottomBallDiag1 =
      createLine(registry, {screen_width, screen_height}, {30, 30});
  middleBottomLinkDiag1 =
      createRope(registry, registry->transforms.get(middleBallDiag1).position,
                 registry->transforms.get(bottomBallDiag1).position);

  // diagonal spring 2
  topBallDiag2 =
      createLine(registry, {screen_width / 2, screen_height / 2}, {30, 30});
  middleBallDiag2 =
      createLine(registry, {screen_width * 3 / 4, screen_height / 4}, {30, 30});
  topMiddleLinkDiag2 =
      createRope(registry, registry->transforms.get(topBallDiag2).position,
                 registry->transforms.get(middleBallDiag2).position);
  bottomBallDiag2 = createLine(registry, {screen_width, 0}, {30, 30});
  middleBottomLinkDiag2 =
      createRope(registry, registry->transforms.get(middleBallDiag2).position,
                 registry->transforms.get(bottomBallDiag2).position);

  // randomize seed for random calls. Without this, the random is predictable
  rng = std::default_random_engine(std::random_device()());
}

void ConstrainedPhysicsWorldSystem::handle_sprite_animation(float delta) {
  // Loop over all animations
}

bool ConstrainedPhysicsWorldSystem::is_over() const { return false; }

void ConstrainedPhysicsWorldSystem::on_key(int key, int action, int mod) {
  (void)mod;
  // Resetting game
  TransformComponent& player_t = registry->transforms.get(current_player);
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    restart_game();
  }

  // Debugging
  if (key == GLFW_KEY_D) {
    if (action == GLFW_RELEASE)
      debugging.in_debug_mode = false;
    else
      debugging.in_debug_mode = true;
  }

  // More input here
  if (registry->deathTimers.entities.empty()) {
    // Left
    if (key == GLFW_KEY_LEFT) {
      Velocity& vel = registry->velocities.get(current_player);
      if (action == GLFW_RELEASE) {
        vel.velocity.x = 0;
      } else {
        vel.velocity.x = -200;
        if (player_t.scale.x > 0) {
        player_t.scale.x *= -1;
      }
      }
    }
    // Right
    if (key == GLFW_KEY_RIGHT) {
      Velocity& vel = registry->velocities.get(current_player);
      if (action == GLFW_RELEASE) {
        vel.velocity.x = 0;
      } else if (action == GLFW_PRESS) {
        vel.velocity.x = 200;
        if (player_t.scale.x < 0) {
        player_t.scale.x *= -1;
      }
      }
    }
    // Up
    if (key == GLFW_KEY_UP) {
      Velocity& vel = registry->velocities.get(current_player);
      if (action == GLFW_RELEASE) {
        vel.velocity.y = 0;
      } else {
        vel.velocity.y = -200;
      }
    }
    // Down
    if (key == GLFW_KEY_DOWN) {
      Velocity& vel = registry->velocities.get(current_player);
      if (action == GLFW_RELEASE) {
        vel.velocity.y = 0;
      } else {
        vel.velocity.y = 200;
      }
    }
  }
}

void ConstrainedPhysicsWorldSystem::on_mouse_move(vec2 mouse_position) {
  (vec2) mouse_position;  // dummy to avoid compiler warning
}

void ConstrainedPhysicsWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
  // Loop over all collisions detected by the physics system
  auto& collisionsRegistry =
      registry->collisions;  // TODO: @Tim, is the reference here needed?
  for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
    // The entity and its collider
    Entity entity = collisionsRegistry.entities[i];
    Entity entity_other = collisionsRegistry.components[i].other;
    (void)entity_other;

    // For now, we are only interested in collisions that involve the salmon
    if (registry->players.has(entity) && !deadYet) {
      deadYet = true;
      registry->deathTimers.emplace(entity);
      registry->velocities.get(current_player).velocity = {0, 0};
      registry->colors.get(entity).r = 255;
      //registry->colors.get(entity).g = 0;
      //registry->colors.get(entity).b = 0;
    }
  }

  // Remove all collisions from this simulation step
  registry->collisions.clear();
}
