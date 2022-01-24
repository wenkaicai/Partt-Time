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

// Game configuration
const size_t MAX_ROCKS = 25;
const size_t ROCK_DELAY_MS = 2000 * 3;

// Create the fish world
MacWorldSystem::MacWorldSystem() : points(0), next_rock_spawn(0.f) {
  // Seeding rng with random device
  // background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
  salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
  salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
  rng = std::default_random_engine(std::random_device()());
}

MacWorldSystem::~MacWorldSystem() {
  // Destroy music components
  // if (background_music != nullptr) Mix_FreeMusic(background_music);
  if (salmon_dead_sound != nullptr) Mix_FreeChunk(salmon_dead_sound);
  if (salmon_eat_sound != nullptr) Mix_FreeChunk(salmon_eat_sound);
  Mix_CloseAudio();

  // Destroy all created components
  registry->clear_all_components();
  registry = nullptr;
  renderer = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void MacWorldSystem::init(std::shared_ptr<MacRegistry> registry,
                          std::shared_ptr<RenderSystem> renderer,
                          std::shared_ptr<MacPhysicsSystem> physics,
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

// Update our game world
bool MacWorldSystem::step(float elapsed_ms_since_last_update) {
  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  // Updating window title with points
  std::stringstream title_ss;
  title_ss << "Points: " << points;
  glfwSetWindowTitle(window, title_ss.str().c_str());

  // Remove debug info from the last step
  while (registry->debugComponents.entities.size() > 0)
    registry->remove_all_components_of(
        registry->debugComponents.entities.back());

  // spawning new rocks
  next_rock_spawn -= elapsed_ms_since_last_update * current_speed * 3;
  if (registry->rocks.components.size() <= MAX_ROCKS && next_rock_spawn < 0.f) {
    // reset timer
    next_rock_spawn =
        (ROCK_DELAY_MS / 2) + uniform_dist(rng) * (ROCK_DELAY_MS / 2);
    // create rock
    Entity entity = createRock(registry, renderer, {0, 0});
    // setting random initial position and constant velocity
    TransformComponent& transform = registry->transforms.get(entity);
    Velocity& velocity = registry->velocities.get(entity);
    transform.position =
        vec2(screen_width + 0.f,  // spawn off-screen
             50.f + uniform_dist(rng) * (screen_height - 100.f));
    float randomY = uniform_dist(rng);
    if (randomY < 0.5) {
      velocity.velocity = vec2(-100.f, -100);
    } else {
      velocity.velocity = vec2(-100.f, 100);
    }
  }

  assert(registry->screenStates.components.size() <= 1);
  ScreenState& screen = registry->screenStates.components[0];

  float min_counter_ms = 3000.f;
  for (Entity entity : registry->deathTimers.entities) {
    // progress timer
    DeathTimer& counter = registry->deathTimers.get(entity);
    counter.counter_ms -= elapsed_ms_since_last_update;
    if (counter.counter_ms < min_counter_ms) {
      min_counter_ms = counter.counter_ms;
    }

    // restart the game once the death timer expired
    if (counter.counter_ms < 0) {
      registry->deathTimers.remove(entity);
      screen.darken_screen_factor = 0;
      on_game_end_callback_ptr();
      return true;
    }
  }

  return true;
}

// Reset the world state to its initial state
void MacWorldSystem::restart_game() {
  camera = Entity();
  Camera& cam = registry->camera.emplace(camera);
  cam.cameraFOV = {1200, 675};

  // Playing background music
  // Mix_PlayMusic(background_music, -1);
  // Debugging for memory/component leaks
  registry->list_all_components();
  printf("Restarting\n");

  // Reset the game speed
  current_speed = 1.f;

  // Remove all entities that we created
  // All that have a motion, we could also iterate over all fish, turtles, ...
  // but that would be more cumbersome
  while (registry->transforms.entities.size() > 0)
    registry->remove_all_components_of(registry->transforms.entities.back());

  // Debugging for memory/component leaks
  registry->list_all_components();
  // background
  createBackground(registry, renderer, {600, 400});
  // Create a new player
  player_salmon = createPlayer(registry, renderer, {100, 200});
  registry->colors.insert(player_salmon, {1, 0.8f, 0.8f});
  // registry->colors.get(player_salmon).r = 0;
  // registry->colors.get(player_salmon).g = 255;
  // registry->colors.get(player_salmon).b = 0;
}

void MacWorldSystem::handleRockPlayerBounce(Entity entity,
                                            Entity entity_other) {
  Velocity& vel1 = registry->velocities.get(entity);
  Velocity& vel2 = registry->velocities.get(entity_other);

  vec2 vel1temp = vel1.velocity;
  vel1.velocity = vel2.velocity;
  vel2.velocity = vel1temp;
}

void MacWorldSystem::handleRockBounce(Entity entity, Entity entity_other) {
  TransformComponent& transform1 = registry->transforms.get(entity);
  TransformComponent& transform2 = registry->transforms.get(entity_other);
  Velocity& vel1 = registry->velocities.get(entity);
  Velocity& vel2 = registry->velocities.get(entity_other);
  float dist = sqrt(pow(transform1.position.x - transform2.position.x, 2) +
                    pow(transform1.position.y - transform2.position.y, 2));
  // source: https://www.youtube.com/watch?v=LPzyNOHY3A4
  float overlap =
      0.5f *
      (dist - 25 - 25);  // Overlap between circles is the distance between
                         // centers minus both radii (half for resolution)
  // displace (static resolution)
  transform1.position.x -=
      overlap * (transform1.position.x - transform2.position.x) / dist;
  transform1.position.y -=
      overlap * (transform1.position.y - transform2.position.y) / dist;
  transform2.position.x +=
      overlap * (transform1.position.x - transform2.position.x) / dist;
  transform2.position.y +=
      overlap * (transform1.position.y - transform2.position.y) / dist;
  // (dynamic resolution)
  // Normal
  float nx = (transform2.position.x - transform1.position.x) / dist;
  float ny = (transform2.position.y - transform1.position.y) / dist;
  // Tangent
  float tx = -ny;
  float ty = nx;

  // Dot Product Tangent
  float dpTan1 = vel1.velocity.x * tx + vel1.velocity.y * ty;
  float dpTan2 = vel2.velocity.x * tx + vel2.velocity.y * ty;

  // Dot Product Normal
  // float dpNorm1 = vel1.velocity.x * nx + vel1.velocity.y * ny;
  // float dpNorm2 = vel2.velocity.x * nx + vel2.velocity.y * ny;

  vel1.velocity.x = tx * dpTan1 + nx;
  vel1.velocity.y = ty * dpTan1 + ny;
  vel2.velocity.x = tx * dpTan2 + nx;
  vel2.velocity.y = ty * dpTan2 + ny;

  float mag1 = sqrt(vel1.velocity.x * vel1.velocity.x +
                    vel1.velocity.y * vel1.velocity.y);
  if (mag1 < 100) {
    vel1.velocity *= 2;
  }
  float mag2 = sqrt(vel2.velocity.x * vel2.velocity.x +
                    vel2.velocity.y * vel2.velocity.y);
  if (mag2 < 100) {
    vel2.velocity *= 2;
  }
  /*
  motion1.velocity.x = motion1.velocity.x * 200 / mag1;
  motion1.velocity.y = motion1.velocity.y * 100 / mag1;
  motion2.velocity.x = motion2.velocity.x * 200 / mag2;
  motion2.velocity.y = motion2.velocity.y * 100 / mag2;
  */
}

// Compute collisions between entities
void MacWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
  auto& collisionsRegistry =
      registry->collisions;  // TODO: @Tim, is the reference here needed?
  for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
    // The entity and its collider
    Entity entity = collisionsRegistry.entities[i];
    Entity entity_other = collisionsRegistry.components[i].other;

    // check rock - rock collisions
    if (registry->rocks.has(entity) && registry->rocks.has(entity_other)) {
      handleRockBounce(entity, entity_other);
    }

    // check player-rock collisions
    if (registry->players.has(entity)) {
      if (registry->rocks.has(entity_other)) {
        // initiate death unless already dying
        if (!registry->deathTimers.has(entity)) {
          // Scream, reset timer, and make the salmon sink
          handleRockPlayerBounce(entity, entity_other);
          registry->deathTimers.emplace(entity);
          Mix_PlayChannel(-1, salmon_dead_sound, 0);
          // registry->motions.get(entity).angle = 3.1415f;
          // registry->motions.get(entity).velocity = { 0, 80 };
          registry->colors.get(entity).r = 0.8;
          registry->colors.get(entity).g = 0.1f;
          registry->colors.get(entity).b = 0.1f;
        }
      }
    }
  }

  // Remove all collisions from this simulation step
  registry->collisions.clear();
}

// On key callback
void MacWorldSystem::on_key(int key, int action, int mod) {
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO A1: HANDLE SALMON MOVEMENT HERE
  // key is of 'type' GLFW_KEY_
  // action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  TransformComponent& player_t = registry->transforms.get(player_salmon);
  // Resetting game
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    int w, h;
    glfwGetWindowSize(window_manager->get_window(), &w, &h);

    restart_game();
  }

  // Debugging
  if (key == GLFW_KEY_D) {
    if (action == GLFW_RELEASE)
      debugging.in_debug_mode = false;
    else
      debugging.in_debug_mode = true;
  }

  // Control the current speed with `<` `>`
  if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&
      key == GLFW_KEY_COMMA) {
    current_speed -= 0.1f;
    printf("Current speed = %f\n", current_speed);
  }
  if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&
      key == GLFW_KEY_PERIOD) {
    current_speed += 0.1f;
    printf("Current speed = %f\n", current_speed);
  }
  current_speed = fmax(0.f, current_speed);

  // Moving the Salmon (if not dead)
  if (registry->deathTimers.entities.empty()) {
    // Left
    if (key == GLFW_KEY_LEFT) {
      Velocity& vel = registry->velocities.get(player_salmon);
      if (action == GLFW_RELEASE) {
        vel.velocity.x = 0;
      } else {
        vel.velocity.x = -200;
      }
      if (player_t.scale.x > 0) {
        player_t.scale.x *= -1;
      }

    }
    // Right
    if (key == GLFW_KEY_RIGHT) {
      Velocity& vel = registry->velocities.get(player_salmon);
      if (action == GLFW_RELEASE) {
        vel.velocity.x = 0;
      } else if (action == GLFW_PRESS) {
        vel.velocity.x = 200;
      }
      if (player_t.scale.x < 0) {
        player_t.scale.x *= -1;
      }

    }
    // Up
    if (key == GLFW_KEY_UP) {
      Velocity& vel = registry->velocities.get(player_salmon);
      if (action == GLFW_RELEASE) {
        vel.velocity.y = 0;
      } else {
        vel.velocity.y = -200;
      }
    }
    // Down
    if (key == GLFW_KEY_DOWN) {
      Velocity& vel = registry->velocities.get(player_salmon);
      if (action == GLFW_RELEASE) {
        vel.velocity.y = 0;
      } else {
        vel.velocity.y = 200;
      }
    }
  }
}

void MacWorldSystem::on_mouse_move(vec2 mouse_position) {
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO A1: HANDLE SALMON ROTATION HERE
  // xpos and ypos are relative to the top-left of the window, the salmon's
  // default facing direction is (1, 0)
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  (vec2) mouse_position;  // dummy to avoid compiler warning
}
