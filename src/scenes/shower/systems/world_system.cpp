// Header
#include "world_system.hpp"

#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const size_t MAX_CATS = 10000;
const size_t MAX_SUSHI = 5000;
// reduece frequency for turtle and fish
const size_t CAT_DELAY_MS = 2000 * 2;
const size_t CLOUD_DELAY_MS = 3000 * 2;
const size_t SUSHI_DELAY_MS = 5000 * 1;

float TURN_FACTOR = 20;
float VISUAL_RANGE = 30;
float PROTECTED_RANGE = 10;
float CENTERING_FACTOR = 0.0;
float AVOID_FACTOR = 0.1;
float MATCHING_FACTOR = 0.01;
float MIN_SPEED = 20;
float MAX_SPEED = 40;

// Create the shower game world
ShowerWorldSystem::ShowerWorldSystem()
    : points(0), next_cat_spawn(0.f), next_sushi_spawn(0.f) {
  // Seeding rng with random device
  rng = std::default_random_engine(std::random_device()());
  // background_music = Mix_LoadMUS(audio_path("fluffing-a-duck.wav").c_str());
  doge_dead_sound = Mix_LoadWAV(audio_path("doge_die.wav").c_str());
  doge_eat_sound = Mix_LoadWAV(audio_path("doge_bark.wav").c_str());
}

ShowerWorldSystem::~ShowerWorldSystem() {
  // Destroy music components
  // if (background_music != nullptr) Mix_FreeMusic(background_music);
  if (doge_dead_sound != nullptr) Mix_FreeChunk(doge_dead_sound);
  if (doge_eat_sound != nullptr) Mix_FreeChunk(doge_eat_sound);
  Mix_CloseAudio();

  // Destroy all created components
  registry->clear_all_components();
  registry = nullptr;
  renderer = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void ShowerWorldSystem::init_swarm() {
  int vw = 1200, vh = 675;

  for (int i = 0; i < 50; i++) {
    float x = vw * uniform_dist(rng);
    float y = vh * uniform_dist(rng);
    auto bird = createBird(registry, {x, y}, {10, 10});

    auto velocity = &registry->velocities.get(bird).velocity;
    velocity->x = MIN_SPEED * uniform_dist(rng);
    velocity->y = MIN_SPEED * uniform_dist(rng);
  }
}

void ShowerWorldSystem::step_swarm(float delta) {
  int vw = 1200, vh = 675;

  for (int i = 0; i < registry->birds.size(); i++) {
    auto bird_i = registry->birds.entities[i];
    vec2* pos_i = &registry->transforms.get(bird_i).position;
    vec2* vel_i = &registry->velocities.get(bird_i).velocity;

    float x_pos_avg = 0;
    float y_pos_avg = 0;
    float x_vel_avg = 0;
    float y_vel_avg = 0;
    float neighboring_birds = 0;
    float close_dx = 0;
    float close_dy = 0;

    for (int j = 0; j < registry->birds.size(); j++) {
      // if birds are the same, continue
      if (i == j) {
        continue;
      }

      auto bird_j = registry->birds.entities[j];
      vec2 pos_j = registry->transforms.get(bird_j).position;
      vec2 vel_j = registry->velocities.get(bird_j).velocity;

      float dx = pos_i->x - pos_j.x;
      float dy = pos_i->y - pos_j.y;

      if (abs(dx) < VISUAL_RANGE && abs(dy) < VISUAL_RANGE) {
        float squared_distance = pow(dx, 2) + pow(dy, 2);

        if (squared_distance < pow(PROTECTED_RANGE, 2)) {
          close_dx += pos_i->x - pos_j.x;
          close_dy += pos_i->y - pos_j.y;
        } else if (squared_distance < pow(VISUAL_RANGE, 2)) {
          x_pos_avg += pos_j.x;
          y_pos_avg += pos_j.y;
          x_vel_avg += vel_j.x;
          y_vel_avg += vel_j.y;
          neighboring_birds++;
        }
      }

      if (neighboring_birds > 0) {
        x_pos_avg = x_pos_avg / neighboring_birds;
        y_pos_avg = y_pos_avg / neighboring_birds;
        x_vel_avg = x_vel_avg / neighboring_birds;
        y_vel_avg = y_vel_avg / neighboring_birds;

        vel_i->x += (x_pos_avg - pos_i->x) * CENTERING_FACTOR +
                    (x_vel_avg - vel_i->x) * MATCHING_FACTOR;

        vel_i->y += (y_pos_avg - pos_i->y) * CENTERING_FACTOR +
                    (y_vel_avg - vel_i->y) * MATCHING_FACTOR;
      }

      vel_i->x += (close_dx * AVOID_FACTOR);
      vel_i->y += (close_dy * AVOID_FACTOR);

      // above top of the screen
      if (pos_i->y < 0) {
        pos_i->y = 0;
        vel_i->y += TURN_FACTOR;
      }

      // below bottom of the screen
      if (pos_i->y > vh) {
        pos_i->y = vh;
        vel_i->y -= TURN_FACTOR;
      }

      // left of the screen
      if (pos_i->x < 0) {
        pos_i->x = 0;
        vel_i->x += TURN_FACTOR;
      }

      // right of the screen
      if (pos_i->x > vw) {
        pos_i->x = vw;
        vel_i->x -= TURN_FACTOR;
      }

      float speed = sqrt(pow(vel_i->x, 2) + pow(vel_i->y, 2));

      if (speed < MIN_SPEED) {
        vel_i->x = (vel_i->x / speed) * MIN_SPEED;
        vel_i->y = (vel_i->y / speed) * MIN_SPEED;
      }

      if (speed > MAX_SPEED) {
        vel_i->x = (vel_i->x / speed) * MAX_SPEED;
        vel_i->y = (vel_i->y / speed) * MAX_SPEED;
      }
    }

    pos_i->x += vel_i->x / delta;
    pos_i->y += vel_i->y / delta;
  }
}

void ShowerWorldSystem::init(std::shared_ptr<ShowerRegistry> registry,
                             std::shared_ptr<RenderSystem> renderer,
                             std::shared_ptr<ShowerPhysicsSystem> physics,
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
bool ShowerWorldSystem::step(float elapsed_ms_since_last_update) {
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

  // Removing out of screen entities
  auto& transform_registry = registry->transforms;

  // Remove entities that leave the screen on the bottom
  // Iterate backwards to be able to remove without unterfering with the next
  // object to visit (the containers exchange the last element with the current)
  for (int i = (int)transform_registry.components.size() - 1; i >= 0; --i) {
    TransformComponent& transform = transform_registry.components[i];
    if (transform.position.y - abs(transform.scale.y) / 2 > 750.f) {
      registry->remove_all_components_of(transform_registry.entities[i]);
    }
  }

  Entity enemy_i = registry->enemyAi.entities[0];
  TransformComponent& enemy_transform = registry->transforms.get(enemy_i);
  // Spawning new clouds
  next_cloud_spawn -= elapsed_ms_since_last_update * current_speed;
  if (next_cloud_spawn < 0.f) {
    // Reset timer
    next_cloud_spawn =
        (CLOUD_DELAY_MS / 3) + uniform_dist(rng) * (CLOUD_DELAY_MS / 2);
    // Create cloud
    Entity cloud =
        createCloud(registry, renderer, {1300.0f, rng() % 100 + 100.0f},
                    {rng() % 100 + 250.0f, rng() % 75 + 170.0f});
  }

  // Spawning new cats
  next_cat_spawn -= elapsed_ms_since_last_update * current_speed;
  if (registry->hardShells.components.size() <= MAX_CATS &&
      next_cat_spawn < 0.f) {
    // Reset timer
    next_cat_spawn =
        (CAT_DELAY_MS / 3) + uniform_dist(rng) * (CAT_DELAY_MS / 2);
    // Create cat
    Entity entity =
        createCat(registry, renderer,
                  {enemy_transform.position.x,
                   enemy_transform.position.y + enemy_transform.scale.y / 2});
    Velocity& vel = registry->velocities.get(entity);
    vel.velocity = vec2(0.f, 0.f);
  }

  // Spawning new sushi
  next_sushi_spawn -= elapsed_ms_since_last_update * current_speed;
  if (registry->softShells.components.size() <= MAX_SUSHI &&
      next_sushi_spawn < 0.f) {
    // above
    next_sushi_spawn =
        (SUSHI_DELAY_MS / 2) + uniform_dist(rng) * (SUSHI_DELAY_MS / 2);
    // Create sushi
    Entity entity = createSushi(
        registry, renderer,
        vec2(50.f + uniform_dist(rng) * (screen_width - 100.f), 50.f));
    // Setting random initial position and constant velocity
    Velocity& vel = registry->velocities.get(entity);
    vel.velocity = vec2(0.f, 0.f);
  }

  // Processing the player state
  assert(registry->screenStates.components.size() <= 1);

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
      on_game_end_callback_ptr();
      return true;
    }
  }

  float min_counterl_ms = 1000.f;
  for (Entity entity : registry->lightUps.entities) {
    // progress timer
    LightUp& counterl = registry->lightUps.get(entity);
    counterl.light_ms -= elapsed_ms_since_last_update;

    if (counterl.light_ms < min_counterl_ms) {
      min_counterl_ms = counterl.light_ms;
    }
    if (counterl.light_ms < 0) {
      registry->lightUps.remove(entity);
      return true;
    }
  }

  step_swarm(elapsed_ms_since_last_update);

  return true;
}

// Reset the world state to its initial state
void ShowerWorldSystem::restart_game() {
  camera = Entity();
  Camera& cam = registry->camera.emplace(camera);
  cam.cameraFOV = {1200, 675};
  // Playing background music for the scene (uncomment when we actually have
  // music)
  // Mix_PlayMusic(background_music, -1);

  // Debugging for memory/component leaks
  registry->list_all_components();
  printf("Restarting\n");

  // Reset the game speed
  current_speed = 1.f;

  // Remove all entities that we created
  while (registry->transforms.entities.size() > 0)
    registry->remove_all_components_of(registry->transforms.entities.back());

  // Debugging for memory/component leaks
  registry->list_all_components();

  // Create a new doge
  createBackground(registry, renderer, {600, 400});
  player_doge = createDoge(registry, renderer, {200, 700});
  registry->colors.insert(player_doge, {1, 0.8f, 0.8f});
  enemy = creatEnemy(registry, renderer, {600, 100});
  block = createBlock(registry, renderer, {600, 620});

  init_swarm();
}

// Compute collisions between entities
void ShowerWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
  Point& player_p = registry->points.get(player_doge);
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
      if (registry->hardShells.has(entity_other)) {
        // initiate death unless already dying
        if (!registry->deathTimers.has(entity)) {
          // Scream, reset timer, and make the salmon sink
          registry->deathTimers.emplace(entity);
          Mix_PlayChannel(-1, doge_dead_sound, 0);
          registry->transforms.get(entity).rotation = 3.1415f;
          registry->velocities.get(entity).velocity = {0, 80};
          registry->colors.remove(entity);
          registry->colors.insert(entity, {1, 0.1f, 0.1f});
        }
      }
      // Checking Player - SoftShell collisions
      else if (registry->softShells.has(entity_other)) {
        if (!registry->deathTimers.has(entity)) {
          // chew, count points, and set the LightUp timer
          registry->remove_all_components_of(entity_other);
          Mix_PlayChannel(-1, doge_eat_sound, 0);
          ++points;
          player_p.player_points = points;
          registry->lightUps.emplace(entity);
        }
      }
    }
  }

  // Remove all collisions from this simulation step
  registry->collisions.clear();
}

// On key callback
void ShowerWorldSystem::on_key(int key, int action, int mod) {
  Velocity& player_v = registry->velocities.get(player_doge);
  TransformComponent& player_t = registry->transforms.get(player_doge);
  Acceleration& player_a = registry->accelerations.get(player_doge);

  // player jump
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (registry->grounded.has(player_doge)) {
      player_v.velocity.y = -350;
      player_t.position.y = player_t.position.y - 0.1;
      player_a.acceleration.y = 300;
      registry->grounded.remove(player_doge);
    }
  }

  // move left
  if (key == GLFW_KEY_LEFT) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      player_v.velocity.x = -400;
      if (player_t.scale.x > 0) {
        player_t.scale.x *= -1;
      }
    }
    if (action == GLFW_RELEASE) {
      player_v.velocity.x = 0;
    }
  }

  // move right
  if (key == GLFW_KEY_RIGHT) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      player_v.velocity.x = 400;
      if (player_t.scale.x < 0) {
        player_t.scale.x *= -1;
      }
    }
    if (action == GLFW_RELEASE) {
      player_v.velocity.x = 0;
    }
  }

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
}

void ShowerWorldSystem::on_mouse_move(vec2 mouse_position) {
  (vec2) mouse_position;  // dummy to avoid compiler warning
}
