// Header
#include "world_system.hpp"

#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

vec2 launchDirection = {0, 0};
float velocityLineDist = 0;
vec2 launchVelocity = {0, 0};
vec2 mousePos = {0, 0};
bool launch = false;
bool shouldDraw = true;

// Create the fish world
PlanitWorldSystem::PlanitWorldSystem()
    : points(0), next_turtle_spawn(0.f), next_fish_spawn(0.f) {
  // Seeding rng with random device
  (void)next_turtle_spawn;
  (void)next_fish_spawn;
  background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
  (void)background_music;

  salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
  salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
  rng = std::default_random_engine(std::random_device()());
  // background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
}

PlanitWorldSystem::~PlanitWorldSystem() {
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

void PlanitWorldSystem::init(std::shared_ptr<PlanitRegistry> registry,
                             std::shared_ptr<RenderSystem> renderer,
                             std::shared_ptr<PlanitPhysicsSystem> physics,
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
bool PlanitWorldSystem::step(float elapsed_ms_since_last_update) {
  handle_sprite_animation(elapsed_ms_since_last_update);
  // Get the screen dimensions
  int screen_width, screen_height;
  glfwGetFramebufferSize(window_manager->get_window(), &screen_width,
                         &screen_height);

  // Updating window title with points
  std::stringstream title_ss;
  title_ss << "Points: " << points;
  glfwSetWindowTitle(window_manager->get_window(), title_ss.str().c_str());

  // Remove debug info from the last step
  while (registry->debugComponents.entities.size() > 0)
    registry->remove_all_components_of(
        registry->debugComponents.entities.back());

  //// Removing out of screen entities
  // auto& motions_registry = registry->motions;

  //// Remove entities that leave the screen on the left side
  //// Iterate backwards to be able to remove without unterfering with the next
  //// object to visit (the containers exchange the last element with the
  /// current)
  // for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
  //  Motion& motion = motions_registry.components[i];
  //  if (motion.position.x + abs(motion.scale.x) < 0.f) {
  //    registry->remove_all_components_of(motions_registry.entities[i]);
  //  }
  //}

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO A3: HANDLE PEBBLE SPAWN HERE
  // DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // Processing the salmon state
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
      shouldDraw = false;
      on_game_end_callback_ptr();
      return true;
    }
  }
  // reduce window brightness if any of the present salmons is dying
  screen.darken_screen_factor = 1 - min_counter_ms / 3000;

  TransformComponent& playerTransform =
      registry->transforms.get(registry->players.entities[0]);
  Velocity& playerVelocity =
      registry->velocities.get(registry->players.entities[0]);
  if (!launch && shouldDraw) {
    // Motion& playerMotion =
    // registry->motions.get(registry->players.entities[0]);
    Entity line = createLine(registry, launchDirection, {velocityLineDist, 10});
    registry->transforms.get(line).rotation =
        atan2(mousePos.y - playerTransform.position.y,
              mousePos.x - playerTransform.position.x);
  }

  if (playerVelocity.velocity.x < 0 && playerTransform.position.x > 100 &&
      playerTransform.position.x < 500) {
    launch = false;
    playerVelocity.velocity = {0, 0};
    registry->deathTimers.emplace(registry->players.entities[0]);
    registry->colors.get(registry->players.entities[0]).r = 255;
    registry->colors.get(registry->players.entities[0]).g = 0;
    registry->colors.get(registry->players.entities[0]).b = 0;
    Mix_PlayChannel(-1, salmon_dead_sound, 0);
  }

  return true;
}

// Reset the world state to its initial state
void PlanitWorldSystem::restart_game() {
  camera = Entity();
  Camera& cam = registry->camera.emplace(camera);
  cam.cameraFOV = {2000, 1125};

  // Playing background music indefinitely
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

  // Create a new salmon
  player_salmon = createPlayer(registry, renderer, {100, 600});
  // registry->motions.get(player_salmon).velocity.y = -100;
  registry->colors.insert(player_salmon, {1, 0.8f, 0.8f});

  // create planet
  createPlanet(registry, renderer, {600, 400});

  // create target
  createTarget(registry, renderer, {600, 600});

  shouldDraw = true;
}

// Compute collisions between entities
void PlanitWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
  auto& collisionsRegistry =
      registry->collisions;  // TODO: @Tim, is the reference here needed?
  for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
    // The entity and its collider
    Entity entity = collisionsRegistry.entities[i];
    Entity entity_other = collisionsRegistry.components[i].other;

    // For now, we are only interested in collisions that involve the salmon
    if (registry->players.has(entity)) {
      // Player& player = registry->players.get(entity);

      // Checking Player - HardShell collisions
      if (registry->planets.has(entity_other)) {
        // initiate death unless already dying
        if (!registry->deathTimers.has(entity)) {
          // Scream, reset timer, and make the salmon sink
          launch = false;
          shouldDraw = false;
          registry->deathTimers.emplace(entity);
          Mix_PlayChannel(-1, salmon_dead_sound, 0);
          // registry->motions.get(entity).angle = 3.1415f;
          registry->velocities.get(entity).velocity = {0, 0};
          registry->colors.get(entity).r = 255;
          //registry->colors.get(entity).g = 0;
          //registry->colors.get(entity).b = 0;
        }
      }
      // Checking Player - SoftShell collisions
      else if (registry->targets.has(entity_other)) {
        if (!registry->deathTimers.has(entity)) {
          //
          // chew, count points, and set the LightUp timer
          if (registry->velocities.get(entity).velocity.x <= 0) {
            registry->deathTimers.emplace(entity);
            launch = false;
            shouldDraw = false;
            registry->velocities.get(entity).velocity = {0, 0};
            //registry->colors.get(entity).r = 0;
            registry->colors.get(entity).g = 255;
            //registry->colors.get(entity).b = 0;
            // registry->remove_all_components_of(entity_other);
            Mix_PlayChannel(-1, salmon_eat_sound, 0);
            ++points;
          } else {
            launch = false;
            shouldDraw = false;
            registry->deathTimers.emplace(entity);
            Mix_PlayChannel(-1, salmon_dead_sound, 0);
            // registry->motions.get(entity).angle = 3.1415f;
            registry->velocities.get(entity).velocity = {0.1, 0};
            registry->colors.get(entity).r = 255;
            //registry->colors.get(entity).g = 0;
            //registry->colors.get(entity).b = 0;
          }
        }
      }
    }
  }

  // Remove all collisions from this simulation step
  registry->collisions.clear();
}

void PlanitWorldSystem::handle_sprite_animation(float delta) {
  // Loop over all collisions detected by the physics system
  auto animationRegistry = &registry->spriteAnimations;
  for (uint i = 0; i < animationRegistry->components.size(); i++) {
    SpriteAnimation& anim = animationRegistry->components[i];

    if (anim.elapsed_time >= (1 / anim.animation_speed) * 1000.0f) {
      anim.frame = (anim.frame + 1) % anim.columns;
      anim.elapsed_time = 0.0f;
    }

    anim.elapsed_time += delta;
  }
}

// On key callback
void PlanitWorldSystem::on_key(int key, int action, int mod) {
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO A1: HANDLE SALMON MOVEMENT HERE
  // key is of 'type' GLFW_KEY_
  // action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
    if (!launch && shouldDraw) {
      launch = true;
      TransformComponent& playerTransform =
          registry->transforms.get(registry->players.entities[0]);
      Velocity& playerVelocity =
          registry->velocities.get(registry->players.entities[0]);
      float x = mousePos.x - playerTransform.position.x;
      float y = mousePos.y - playerTransform.position.y;
      launchDirection = {x / 2, y / 2};
      playerVelocity.velocity = launchDirection;
      shouldDraw = false;
    }
  }
}

void PlanitWorldSystem::on_mouse_move(vec2 mouse_position) {
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // TODO A1: HANDLE SALMON ROTATION HERE
  // xpos and ypos are relative to the top-left of the window, the salmon's
  // default facing direction is (1, 0)
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  mousePos = mouse_position;
  

  TransformComponent& playerTransform = registry->transforms.get(player_salmon);
  float diffX = mouse_position.x - playerTransform.position.x;
  float diffY = mouse_position.y - playerTransform.position.y;

  launchDirection = {(mouse_position.x + playerTransform.position.x) * 0.5,
                     (mouse_position.y + playerTransform.position.y) * 0.5};
  velocityLineDist = sqrt(diffX * diffX + diffY * diffY);
  launchVelocity = mouse_position;

  if (!launch) {
    playerTransform.rotation = atan2(diffY, diffX);
  }
}
