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
const int NUM_PUPPIES = 5;
const int NUM_FOOD_BOWLS = 2;
const int NUM_WATER_BOWLS = 2;
const int NUM_CHEW_TOYS = 2;

float game_time = 0;
const float GAME_DURATION = 45 * 1000;        // 45 seconds
const float DEPLETION_TIME = 30 * 1000;       // 20 seconds
const float SATURATION_TIME = 5 * 1000;       // 5 seconds
const float BOWL_DEPLETION_TIME = 10 * 1000;  // 10 seconds

const float MOVEMENT_SPEED = 100;
const float WANDER_PROBABILITY = 0.001;

const vec2 GESTURE_START = vec2(150, 168.75);
const vec2 GESTURE_MIDDLE = vec2(600, 506.25);
const vec2 GESTURE_END = vec2(1050, 168.75);
std::vector<vec2> gesture_points;
int current_gesture_point = 0;

vec2 cached_bowl_position;

vec2 last_mouse_pos = vec2(0.f, 0.f);

DaycareWorldSystem::DaycareWorldSystem() {
  // Seeding rng with random device
  rng = std::default_random_engine(std::random_device()());
}

DaycareWorldSystem::~DaycareWorldSystem() {
  // Destroy all created components
  registry->clear_all_components();

  // clean up all the shared pointers
  renderer = nullptr;
  registry = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void DaycareWorldSystem::init(std::shared_ptr<DaycareRegistry> registry,
                              std::shared_ptr<RenderSystem> renderer,
                              std::shared_ptr<DaycarePhysicsSystem> physics,
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

void DaycareWorldSystem::update_progress_bars(float delta) {
  for (int i = 0; i < registry->puppies.size(); i++) {
    auto puppy = registry->puppies.entities[i];
    auto taskProgresses = &registry->puppies.components[i];
    auto puppyTransform = &registry->transforms.get(puppy);

    double depletion_amount = (double)(delta / DEPLETION_TIME);
    double saturation_amount = (double)(delta / SATURATION_TIME);

    taskProgresses->eating =
        max((double)(taskProgresses->eating - depletion_amount), 0.0);
    taskProgresses->drinking =
        max(taskProgresses->drinking - depletion_amount, 0.0);
    taskProgresses->playing =
        max(taskProgresses->playing - depletion_amount, 0.0);

    for (int j = 0; j < registry->foodBowls.size(); j++) {
      auto foodBowl = registry->foodBowls.entities[j];
      auto amount = &registry->foodBowls.components[j].amount;
      auto bowlTransform = &registry->transforms.get(foodBowl);
      bool dx = abs(puppyTransform->position.x - bowlTransform->position.x) <=
                bowlTransform->scale.x * 2;
      bool dy = abs(puppyTransform->position.y - bowlTransform->position.y) <=
                bowlTransform->scale.y * 2;
      if (dx && dy && *amount > 0) {
        taskProgresses->eating =
            min(taskProgresses->eating + saturation_amount, 1.0);
        *amount = max(0.f, *amount - (delta / BOWL_DEPLETION_TIME));
        break;
      }
    }

    for (int j = 0; j < registry->waterBowls.size(); j++) {
      auto waterBowl = registry->waterBowls.entities[j];
      auto amount = &registry->waterBowls.components[j].amount;
      auto bowlTransform = &registry->transforms.get(waterBowl);
      bool dx = abs(puppyTransform->position.x - bowlTransform->position.x) <=
                bowlTransform->scale.x * 2;
      bool dy = abs(puppyTransform->position.y - bowlTransform->position.y) <=
                bowlTransform->scale.y * 2;
      if (dx && dy && *amount > 0) {
        taskProgresses->drinking =
            min(taskProgresses->drinking + saturation_amount, 1.0);
        *amount = max(0.f, *amount - (delta / BOWL_DEPLETION_TIME));
        break;
      }
    }

    for (int j = 0; j < registry->chewToys.size(); j++) {
      auto chewToy = registry->chewToys.entities[j];
      auto toyTransform = &registry->transforms.get(chewToy);
      bool dx = abs(puppyTransform->position.x - toyTransform->position.x) <=
                toyTransform->scale.x * 2;
      bool dy = abs(puppyTransform->position.y - toyTransform->position.y) <=
                toyTransform->scale.y * 2;
      if (dx && dy) {
        taskProgresses->playing =
            min(taskProgresses->playing + saturation_amount, 1.0);
        break;
      }
    }

    for (int j = i * 3; j < i * 3 + 3; j++) {
      auto bar = registry->progressBars.entities[j];
      auto type = registry->progressBars.components[j].type;
      auto barTransfrom = &registry->transforms.get(bar);
      switch (type) {
        case DC_ACTIVITY::EATING:
          barTransfrom->position =
              puppyTransform->position -
              vec2(0.f, puppyTransform->scale.y / 2 + barTransfrom->scale.y);
          barTransfrom->scale.x =
              puppyTransform->scale.x * 1.5 * taskProgresses->eating;
          break;
        case DC_ACTIVITY::DRINKING:
          barTransfrom->position = puppyTransform->position -
                                   vec2(0.f, puppyTransform->scale.y / 2 +
                                                 2 * barTransfrom->scale.y);
          barTransfrom->scale.x =
              puppyTransform->scale.x * 1.5 * taskProgresses->drinking;
          break;
        case DC_ACTIVITY::PLAYING:
          barTransfrom->position = puppyTransform->position -
                                   vec2(0.f, puppyTransform->scale.y / 2 +
                                                 3 * barTransfrom->scale.y);
          barTransfrom->scale.x =
              puppyTransform->scale.x * 1.5 * taskProgresses->playing;
          break;
      }
    }
  }
}

void DaycareWorldSystem::update_bowls() {
  for (int i = 0; i < registry->foodBowls.size(); i++) {
    auto entity = registry->foodBowls.entities[i];
    auto amount = registry->foodBowls.components[i].amount;
    auto request = &registry->renderRequests.get(entity);
    if (amount == 0.f) {
      request->used_texture = TEXTURE_ASSET_ID::FOOD_BOWL_EMPTY;
    } else {
      request->used_texture = TEXTURE_ASSET_ID::FOOD_BOWL_FULL;
    }
  }

  for (int i = 0; i < registry->waterBowls.size(); i++) {
    auto entity = registry->waterBowls.entities[i];
    auto amount = registry->waterBowls.components[i].amount;
    auto request = &registry->renderRequests.get(entity);
    if (amount == 0.f) {
      request->used_texture = TEXTURE_ASSET_ID::WATER_BOWL_EMPTY;
    } else {
      request->used_texture = TEXTURE_ASSET_ID::WATER_BOWL_FULL;
    }
  }
}

void DaycareWorldSystem::update_debug_lines(float delta) {
  for (int i = 0; i < registry->puppies.size(); i++) {
    auto puppy = registry->puppies.entities[i];
    auto tPuppy = &registry->transforms.get(puppy);

    for (int j = i * 8; j < i * 8 + 8; j++) {
      auto line = registry->debugLines.entities[j];
      auto tLine = &registry->transforms.get(line);
      auto rLine = -tLine->rotation;
      tLine->position.x = tPuppy->position.x + tPuppy->scale.x * cos(rLine);
      tLine->position.y = tPuppy->position.y - tPuppy->scale.y * sin(rLine);
    }
  }

  for (int i = 0; i < registry->puppies.size(); i++) {
    auto puppy = registry->puppies.entities[i];
    auto transfrom = registry->transforms.get(puppy);
    if (!registry->targets.has(puppy)) {
      bool shouldWander = uniform_dist(rng) < WANDER_PROBABILITY;
      if (shouldWander) {
        auto target = &registry->targets.emplace(puppy);
        target->target = get_random_window_position({0, 0});

        // std::cout << "New target: " << target->target.x << ","
        //           << target->target.y << std::endl;

        auto motion = &registry->velocities.get(puppy);
        vec2 dir = normalize(target->target - transfrom.position);
        motion->velocity = dir * MOVEMENT_SPEED;
      }
    }
  }
}

// Update our game world
bool DaycareWorldSystem::step(float delta) {
  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  // add step functions here

  handle_collisions();

  update_debug_lines(delta);
  update_progress_bars(delta);
  update_bowls();

  game_time += delta;
  if (game_time > GAME_DURATION) {
    std::cout << "game over" << std::endl;
    on_game_end_callback_ptr();
  }

  return true;
}

vec2 DaycareWorldSystem::get_random_window_position(vec2 offset) {
  int vw, vh;
  glfwGetFramebufferSize(window_manager->get_window(), &vw, &vh);

  float x = vw * uniform_dist(rng);
  float y = vh * uniform_dist(rng);

  return vec2(x, y);
};

void DaycareWorldSystem::initializeEntities() {
  int vw, vh;
  glfwGetFramebufferSize(window_manager->get_window(), &vw, &vh);

  for (int i = 0; i < 10; i++) {
    float dx = GESTURE_MIDDLE.x - GESTURE_START.x;
    float dy = GESTURE_MIDDLE.y - GESTURE_START.y;
    auto point =
        vec2(GESTURE_START.x + (dx / 10) * i, GESTURE_START.y + (dy / 10) * i);
    gesture_points.push_back(point);
  }

  for (int i = 0; i < 11; i++) {
    float dx = GESTURE_END.x - GESTURE_MIDDLE.x;
    float dy = GESTURE_END.y - GESTURE_MIDDLE.y;
    auto point = vec2(GESTURE_MIDDLE.x + (dx / 10) * i,
                      GESTURE_MIDDLE.y + (dy / 10) * i);
    gesture_points.push_back(point);
  }

  for (int i = 0; i < NUM_PUPPIES; i++) {
    float radius = 15;
    auto pos = get_random_window_position({radius, radius});
    auto size = vec2(radius * 2, radius * 2);
    auto puppy = createDoge(registry, renderer, pos);

    registry->puppies.emplace(puppy);
    registry->velocities.emplace(puppy);

    // draw debug lines
    for (int j = 0; j < 8; j++) {
      // create debug line
      float rot = (M_PI / 4) * j;
      vec2 linePos;
      linePos.x = pos.x + radius * cos(rot);
      linePos.y = pos.y - radius * sin(rot);
      auto line = createCircle(registry, linePos, {radius * 2, 2});
      registry->debugLines.emplace(line);

      // update rotation
      auto transform = &registry->transforms.get(line);
      transform->rotation = -rot;

      // set color
      auto color = &registry->colors.emplace(line);
      *color = vec3(0, 0, 1);

      if (!debugging.in_debug_mode) {
        registry->renderRequests.remove(line);
      }
    }

    auto lineScale = vec2(radius * 2, radius / 3);

    auto eatLinePos = pos - vec2(0.f, radius / 2 + lineScale.y);
    auto eatLine = createCircle(registry, eatLinePos, lineScale);
    registry->colors.insert(eatLine, {0, 1, 0});
    auto& eatProgress = registry->progressBars.emplace(eatLine);
    eatProgress.type = DC_ACTIVITY::EATING;

    auto drinkLinePos = eatLinePos - vec2(0.f, lineScale.y);
    auto drinkLine = createCircle(registry, drinkLinePos, lineScale);
    registry->colors.insert(drinkLine, {0, 0, 1});
    auto& drinkProgress = registry->progressBars.emplace(drinkLine);
    drinkProgress.type = DC_ACTIVITY::DRINKING;

    auto playLinePos = drinkLinePos - vec2(0.f, lineScale.y);
    auto playLine = createCircle(registry, playLinePos, lineScale);
    registry->colors.insert(playLine, {1, 0, 0});
    auto& playProgres = registry->progressBars.emplace(playLine);
    playProgres.type = DC_ACTIVITY::PLAYING;
  }

  for (int i = 0; i < NUM_CHEW_TOYS; i++) {
    float radius = 30;
    auto pos = get_random_window_position({radius, radius});
    createChewToys(registry, renderer, pos);
  }

  for (int i = 0; i < NUM_CHEW_TOYS; i++) {
    float radius = 30;
    auto pos = get_random_window_position({radius, radius});
    createFoodBowl(registry, renderer, pos);
  }

  for (int i = 0; i < NUM_WATER_BOWLS; i++) {
    float radius = 30;
    auto pos = get_random_window_position({radius, radius});
    createWaterBowl(registry, renderer, pos);
  }
}

// Reset the world state to its initial state
void DaycareWorldSystem::restart_game() {
  int vw, vh;
  glfwGetFramebufferSize(window_manager->get_window(), &vw, &vh);

  if (registry->camera.size() == 0) {
    camera = Entity();
    Camera* cam = &registry->camera.emplace(camera);
    cam->cameraPosition = vec2(vw / 2, vh / 2);
    cam->cameraTarget = vec2(vw / 2, vh / 2);
    cam->cameraFOV = vec2(vw, vh);
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

  // randomize seed for random calls. Without this, the random is predictable
  rng = std::default_random_engine(std::random_device()());

  // reset the clock
  game_time = 0;

  // rest gesture
  current_gesture_point = 0;
  gesture_points.clear();

  // draw all the assets
  createBackground(registry, renderer, {720, 540});
  initializeEntities();
}

void DaycareWorldSystem::handle_sprite_animation(float delta) {
  // Loop over all collisions detected by the physics system
  auto animationRegistry = &registry->spriteAnimations;
  for (uint i = 0; i < animationRegistry->components.size(); i++) {
    auto puppy = animationRegistry->entities[i];
    SpriteAnimation& anim = animationRegistry->components[i];
    auto velocity = registry->velocities.get(puppy).velocity;

    if (velocity.x == 0 && velocity.y == 0) {
      anim.animation = 0;
    } else if (velocity.x < 0) {
      anim.animation = 1;
    } else if (velocity.x > 0) {
      anim.animation = 2;
    }

    if (anim.elapsed_time >= (1 / anim.animation_speed) * 1000.0f) {
      anim.frame = (anim.frame + 1) % anim.columns;
      anim.elapsed_time = 0.0f;
    }

    anim.elapsed_time += delta;
  }
}

bool DaycareWorldSystem::is_over() const { return true; }

void DaycareWorldSystem::remove_gesture_path() {
  for (int i = 0; i < registry->renderRequests.size(); i++) {
    auto entity = registry->renderRequests.entities[i];
    auto request = &registry->renderRequests.components[i];
    if (request->used_texture == TEXTURE_ASSET_ID::BKGD_GESTURE) {
      registry->remove_all_components_of(entity);
    }
  }
}

void DaycareWorldSystem::on_key(int key, int action, int mod) {
  (void)mod;
  // Resetting game
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    restart_game();
  }

  // Debugging
  if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    restart_game();
  }

  if (key == GLFW_KEY_F) {
    if (action == GLFW_PRESS) {
      float d = 99999999;
      Entity closest;

      for (int i = 0; i < registry->puppies.size(); i++) {
        auto puppy = registry->puppies.entities[i];
        auto transform = registry->transforms.get(puppy);
        auto pos = transform.position;
        auto scale = transform.scale;
        float dx = abs(last_mouse_pos.x - pos.x);
        float dy = abs(last_mouse_pos.y - pos.y);
        if (dx <= scale.x / 2.f && dy <= scale.y / 2.f) {
          float dist = sqrt(pow(dx, 2) + pow(dy, 2));
          if (dist < d) {
            d = dist;
            closest = puppy;
          }
        }
      }

      for (int i = 0; i < registry->foodBowls.size(); i++) {
        auto bowl = registry->foodBowls.entities[i];
        auto amount = registry->foodBowls.components[i].amount;
        auto transform = registry->transforms.get(bowl);
        auto pos = transform.position;
        auto scale = transform.scale;
        float dx = abs(last_mouse_pos.x - pos.x);
        float dy = abs(last_mouse_pos.y - pos.y);
        if (dx <= scale.x / 2.f && dy <= scale.y / 2.f) {
          float dist = sqrt(pow(dx, 2) + pow(dy, 2));
          if (dist < d && amount == 0) {
            d = dist;
            closest = bowl;
          }
        }
      }

      for (int i = 0; i < registry->waterBowls.size(); i++) {
        auto bowl = registry->waterBowls.entities[i];
        auto amount = registry->waterBowls.components[i].amount;
        auto transform = registry->transforms.get(bowl);
        auto pos = transform.position;
        auto scale = transform.scale;
        float dx = abs(last_mouse_pos.x - pos.x);
        float dy = abs(last_mouse_pos.y - pos.y);
        if (dx <= scale.x / 2.f && dy <= scale.y / 2.f) {
          float dist = sqrt(pow(dx, 2) + pow(dy, 2));
          if (dist < d && amount == 0) {
            d = dist;
            closest = bowl;
          }
        }
      }

      if (d != 99999999) {
        registry->draggables.emplace(closest);
        if (registry->foodBowls.has(closest) ||
            registry->waterBowls.has(closest)) {
          auto transform = registry->transforms.get(closest);
          cached_bowl_position = transform.position;
          createGesturePath(registry, renderer, {600, 337.5});
        }
      }
    }

    if (action == GLFW_RELEASE) {
      if (registry->draggables.size() > 0) {
        auto draggable = registry->draggables.entities[0];
        registry->targets.remove(draggable);
        registry->draggables.remove(draggable);
        remove_gesture_path();
      }
    }
  }
}

void DaycareWorldSystem::on_mouse_move(vec2 mouse_position) {
  last_mouse_pos = mouse_position;

  for (int i = 0; i < registry->draggables.size(); i++) {
    auto entity = registry->draggables.entities[i];
    auto transform = &registry->transforms.get(entity);
    transform->position = mouse_position;

    if (!registry->foodBowls.has(entity) && !registry->waterBowls.has(entity)) {
      continue;
    }

    bool nearPrevPoint = current_gesture_point == 0 ||
                         length(gesture_points.at(current_gesture_point - 1) -
                                mouse_position) < 100;
    bool nearCurrPoint =
        length(gesture_points.at(current_gesture_point) - mouse_position) < 100;

    if (!nearPrevPoint && !nearCurrPoint) {
      current_gesture_point = 0;
    } else if (nearCurrPoint) {
      current_gesture_point++;
    }

    if (current_gesture_point == gesture_points.size()) {
      current_gesture_point = 0;

      if (registry->foodBowls.has(entity)) {
        auto bowl = &registry->foodBowls.get(entity);
        bowl->amount = 1;
      }

      if (registry->waterBowls.has(entity)) {
        auto bowl = &registry->waterBowls.get(entity);
        bowl->amount = 1;
      }

      auto transform = &registry->transforms.get(entity);
      transform->position = cached_bowl_position;

      registry->draggables.remove(entity);

      remove_gesture_path();
    }
  }
}

void DaycareWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
}
