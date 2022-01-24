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

BoardWorldSystem::BoardWorldSystem() {
  // Seeding rng with random device
  rng = std::default_random_engine(std::random_device()());

  background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
  space_land = Mix_LoadWAV(audio_path("UI_41.wav").c_str());
  plus_coins = Mix_LoadWAV(audio_path("plus_coin.wav").c_str());
  minus_coins = Mix_LoadWAV(audio_path("minus_coin.wav").c_str());
  explosion1 = Mix_LoadWAV(audio_path("explosion_1.wav").c_str());
  powerup1 = Mix_LoadWAV(audio_path("power_up_1.wav").c_str());
  next_player = Mix_LoadWAV(audio_path("next_player.wav").c_str());
  dice_roll = Mix_LoadWAV(audio_path("dice_roll2.wav").c_str());
  dice_hit = Mix_LoadWAV(audio_path("dice_hit.wav").c_str());
  error = Mix_LoadWAV(audio_path("error_1.wav").c_str());
  spring = Mix_LoadWAV(audio_path("spring.wav").c_str());
  bark = Mix_LoadWAV(audio_path("doge_bark.wav").c_str());
  whimper = Mix_LoadWAV(audio_path("doge_whimper.wav").c_str());

  if (background_music == nullptr || space_land == nullptr ||
      plus_coins == nullptr || minus_coins == nullptr ||
      next_player == nullptr || dice_roll == nullptr) {
    fprintf(stderr,
            "Failed to load sounds make sure the data directory is present");
    // return nullptr;
  }

  // restart_game();
}

BoardWorldSystem::~BoardWorldSystem() {
  // Destroy all created components
  registry->clear_all_components();
  registry = nullptr;
  renderer = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void BoardWorldSystem::init(std::shared_ptr<BoardRegistry> registry,
                            std::shared_ptr<RenderSystem> renderer,
                            std::shared_ptr<BoardPhysicsSystem> physics,
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
bool BoardWorldSystem::step(float delta) {
  // update camera
  registry->camera.get(camera).cameraTarget =
      registry->transforms.get(current_player).position;
  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  // Updating window title with points
  std::stringstream title_ss;
  PlayerBoardMovement &pbm = registry->playerBoardMovements.get(current_player);

  title_ss << "Points: " << registry->players.get(current_player).points
           << " Roll Count Left: " << pbm.roll_count_left;
  glfwSetWindowTitle(window, title_ss.str().c_str());

  // display number of points
  for (uint i = 0; i < registry->players.entities.size(); i++) {
    Entity entity = registry->players.entities[i];
    renderer->add_text_to_be_rendered(
        {std::to_string(registry->players.get(entity).points)}, positions[i], 1,
        vec3(1.0, 1.0, 1.0), RenderSystem::FONTS::LIGHT, 0);
  }

  // update player standings
  if (registry->players.size() == 1) {
    registry->UIpasses.get(p1_standing).display = 1;
    registry->UIpasses.get(p2_standing).display = 0;
    registry->UIpasses.get(p3_standing).display = 0;
    registry->UIpasses.get(p4_standing).display = 0;
  } else if (registry->players.size() == 2) {
    registry->UIpasses.get(p1_standing).display = 1;
    registry->UIpasses.get(p2_standing).display = 1;
    registry->UIpasses.get(p3_standing).display = 0;
    registry->UIpasses.get(p4_standing).display = 0;
  } else if (registry->players.size() == 3) {
    registry->UIpasses.get(p1_standing).display = 1;
    registry->UIpasses.get(p2_standing).display = 1;
    registry->UIpasses.get(p3_standing).display = 1;
    registry->UIpasses.get(p4_standing).display = 0;
  } else if (registry->players.size() == 4) {
    registry->UIpasses.get(p1_standing).display = 1;
    registry->UIpasses.get(p2_standing).display = 1;
    registry->UIpasses.get(p3_standing).display = 1;
    registry->UIpasses.get(p4_standing).display = 1;
  }
  for (uint i = 0; i < registry->players.entities.size(); i++) {
    Entity cur = registry->players.entities[i];
    int below = 0;
    for (uint j = 0; j < registry->players.entities.size(); j++) {
      if (i == j) continue;
      Entity other = registry->players.entities[j];
      if (registry->players.get(cur).points <
          registry->players.get(other).points) {
        below += 1;
      }
    }
    registry->players.get(cur).standing = below;
  }
  // actually change texture on standings
  standings = {p1_standing, p2_standing, p3_standing, p4_standing};
  for (uint i = 0; i < registry->players.entities.size(); i++) {
    Entity entity = registry->players.entities[i];
    registry->spriteAnimations.get(standings[i]).frame =
        registry->players.get(entity).standing;
  }

  // Remove debug info from the last step
  while (registry->debugComponents.entities.size() > 0)
    registry->remove_all_components_of(
        registry->debugComponents.entities.back());

  // UI focus states (blurs everything and darkens everything but UI
  // elements).... eventually
  assert(registry->screenStates.components.size() <= 1);
  ScreenState &screen = registry->screenStates.components[0];

  if (board_state == BOARD_STATE::PICKING_ITEM) {
    screen.screen_brightness = 0.5;
    screen.blur_fullscreen = true;
    screen.blur_size = 0.025;
  } else if (board_state == BOARD_STATE::WAITING_CONFIRMATION) {
    screen.blur_fullscreen = true;
    screen.blur_size = 0.025;
    std::string current_player_text =
        "Player " +
        std::to_string(registry->players.get(current_player).player_id);
    renderer->add_text_to_be_rendered({current_player_text}, vec2(0.45, 0.43),
                                      1, vec3(1, 1, 1),
                                      RenderSystem::FONTS::BOLD, 0);
  } else {
    screen.screen_brightness = 1.0;
    screen.blur_fullscreen = false;
    screen.blur_size = 0.02;
  }

  UIPass &item1_ui = registry->UIpasses.get(item_1_display);
  item1_ui.display = board_state == BOARD_STATE::PICKING_ITEM;
  UIPass &item2_ui = registry->UIpasses.get(item_2_display);
  item2_ui.display = board_state == BOARD_STATE::PICKING_ITEM;
  UIPass &item3_ui = registry->UIpasses.get(item_3_display);
  item3_ui.display = board_state == BOARD_STATE::PICKING_ITEM;
  UIPass &item1_card_ui = registry->UIpasses.get(item_1_card);
  item1_card_ui.display = board_state == BOARD_STATE::PICKING_ITEM;
  UIPass &item2_card_ui = registry->UIpasses.get(item_2_card);
  item2_card_ui.display = board_state == BOARD_STATE::PICKING_ITEM;
  UIPass &item3_card_ui = registry->UIpasses.get(item_3_card);
  item3_card_ui.display = board_state == BOARD_STATE::PICKING_ITEM;

  Player &player = registry->players.get(current_player);
  // update item picking entities (entity will never be deleted, just
  // transparent if not being shown, and updates per active player)
  SpriteAnimation &item1_anim = registry->spriteAnimations.get(item_1_display);
  item1_anim.frame =
      (int)player.item_1 - 1;  // we subtract 1 because index 0 of items is NONE
                               // and not an actual item
  SpriteAnimation &item2_anim = registry->spriteAnimations.get(item_2_display);
  item2_anim.frame = (int)player.item_2 - 1;
  SpriteAnimation &item3_anim = registry->spriteAnimations.get(item_3_display);
  item3_anim.frame = (int)player.item_3 - 1;

  // Update text help for each board state
  UIPass &confirm_text_comp = registry->UIpasses.get(confirm_text);
  UIPass &back_text_comp = registry->UIpasses.get(back_text);
  UIPass &dice_text_comp = registry->UIpasses.get(dice_text);
  UIPass &item_text_comp = registry->UIpasses.get(item_text);
  UIPass &save_text_comp = registry->UIpasses.get(save_text);
  UIPass &load_text_comp = registry->UIpasses.get(load_text);

  if (board_state == BOARD_STATE::ROLLING) {
    dice_text_comp.display = 1;
    item_text_comp.display = 1;
  } else {
    dice_text_comp.display = 0;
    item_text_comp.display = 0;
  }

  if (board_state == BOARD_STATE::PICKING_ITEM) {
    back_text_comp.display = 1;
  } else {
    back_text_comp.display = 0;
  }

  if (board_state == BOARD_STATE::WAITING_CONFIRMATION) {
    confirm_text_comp.display = 1;
  } else {
    confirm_text_comp.display = 0;
  }

  if (board_state == BOARD_STATE::ROLLING ||
      board_state == BOARD_STATE::WAITING_CONFIRMATION) {
    save_text_comp.display = 1;
    load_text_comp.display = 1;
  } else {
    save_text_comp.display = 0;
    load_text_comp.display = 0;
  }

  if (board_state == BOARD_STATE::MOVING) {
    PlayerBoardMovement &pbm =
        registry->playerBoardMovements.get(current_player);

    // This is to begin movement, not to continue it. That is handled during the
    // collision checks
    if ((pbm.current_space == pbm.target_space) && (pbm.roll_count_left > 0)) {
      pbm.target_space = registry->spaces.get(pbm.current_space).next_space;
    }
  }

  // Update dice roll info
  if (board_state == BOARD_STATE::ROLLING) {
    SpriteAnimation &anim = registry->spriteAnimations.get(dice_info);
    anim.animation = 1;
    int roll = rng() % (roll_max - roll_min + 1) + roll_min;
    anim.frame = roll;
    if (!Mix_Playing(2)) {
      Mix_PlayChannel(2, dice_roll, 0);
    }
  } else if (board_state == BOARD_STATE::MOVING ||
             board_state == BOARD_STATE::DICE_HIT_ANIMATION ||
             board_state == BOARD_STATE::PICKING_DIRECTION) {
    SpriteAnimation &anim = registry->spriteAnimations.get(dice_info);
    anim.animation = 0;
    anim.frame =
        registry->playerBoardMovements.get(current_player).roll_count_left;
  } else {
    SpriteAnimation &anim = registry->spriteAnimations.get(dice_info);
    anim.animation = 0;
    anim.frame = 0;
  }

  // Handle animation time outs (ie. waiting for animations/SFX to complete
  // before moving to the next state
  if (animation_timeout - delta < 0.0f) {
    animation_timeout = 0.0f;

    if (board_state == BOARD_STATE::DICE_HIT_ANIMATION) {
      board_state = BOARD_STATE::MOVING;
    } else if (board_state == BOARD_STATE::LANDED_ANIMATION) {
      update_active_player();
      Mix_PlayChannel(2, next_player, 0);
      board_state = BOARD_STATE::WAITING_CONFIRMATION;
    } else if (board_state == BOARD_STATE::PICKED_ITEM_ANIMATION) {
      board_state = BOARD_STATE::ROLLING;
    } else if (board_state == BOARD_STATE::GOT_ITEM_ANIMATION) {
      board_state = BOARD_STATE::WAITING_CONFIRMATION;
    }
  } else {
    animation_timeout -= delta;
  }

  // Handle character animations based on current velocities
  // TODO: allow for sudden animations, like jumping.
  auto playerRegistry = registry->players;
  for (uint i = 0; i < playerRegistry.components.size(); i++) {
    // The entity and its collider
    Entity entity = playerRegistry.entities[i];
    Velocity &vel = registry->velocities.get(entity);
    SpriteAnimation &anim = registry->spriteAnimations.get(entity);

    if (dot(vel.velocity, vel.velocity) < 0.1f) {
      anim.animation = 0;
    } else if (vel.velocity[0] < 0) {
      anim.animation = 1;
    } else if (vel.velocity[0] > 0) {
      anim.animation = 2;
    } else {
      anim.animation = 0;  // temp will add vertical animations soon
    }
  }

  // HANDLE PARTICLE SYSTEMS
  auto particleSystemRegistry = registry->particleSystems;
  for (uint i = 0; i < particleSystemRegistry.entities.size(); i++) {
    // The entity and its collider
    Entity entity = particleSystemRegistry.entities[i];
    ParticleSystem &ps = registry->particleSystems.get(entity);
    TransformComponent &transform = registry->transforms.get(entity);

    // increase life
    ps.life += delta;
    ps.particleSpawnTimeout -= delta;

    uint numParticlesToSpawn =
        (int)(-ps.particleSpawnTimeout / ps.spawningRate);

    // printf("there are %d particles in the system\n",
    // ps.particles_alive.size());

    // handle particle's life goals (ie. death, move)
    for (uint i = 0; i < ps.particles_alive.size(); i++) {
      // only look at alive particles
      if (ps.particles_alive[i] == 1) {
        ps.particles_life[i] -= delta;

        // handle death
        if (ps.particles_life[i] < 0) {
          ps.particles_alive[i] = 0;
          ps.num_alive -= 1;
          ps.particles_size[i] = 0;  // hack to not diaply dead particles
        }
      }
      // moved this outside because there is a moment where particles 'die' but
      // are still displayed and they are completely still handle position
      // update based on velocity and velocity with acceleration
      ps.particles_position[i] += ps.particles_velocity[i] * delta / 1000.0f;
      ps.particles_velocity[i] += ps.particleAcceleration * delta / 1000.0f;
    }

    // handle particle spawning
    if (ps.particleSpawnTimeout <= 0 && ps.life < ps.particleSystemLifetime) {
      for (uint i = 0; i < numParticlesToSpawn; i++) {  // agnostic to fps
        // note to future me: we can add randomness to spawning position here
        vec2 pos = transform.position;
        float randomAngle = ((rng() % ((ps.coneAngle + 1) * 100)) / 100.0f) +
                            (ps.spawningAngle - ps.coneAngle / 2.0f);
        // convert angle to vector direction
        float y = sin(randomAngle * 3.14159 / 180.0f);
        float x = cos(randomAngle * 3.14159 / 180.0f);

        // choose random velocity to spawn particle in
        vec2 vel =
            vec2(x, y) * (ps.initialSpeed *
                          (1 - ((rng() % 100) / 100.0f) * ps.speedRandomness));
        // choose random size to spawn particle with
        float size = ps.particleSize *
                     (1 - ((rng() % 100) / 100.0f) * ps.particleSizeRandomness);
        // choose random lifetime for particle
        float lifetime = ps.initialLifetime *
                         (1 - ((rng() % 100) / 100.0f) * ps.lifetimeRandomness);

        ps.particles_alive.push_back(1);
        ps.particles_position.push_back(pos);
        ps.particles_velocity.push_back(vel);
        ps.particles_life.push_back(lifetime);
        ps.particles_size.push_back(size);

        ps.particleSpawnTimeout = ps.spawningRate;
        ps.num_alive += 1;
      }
    }

    // handle death of particle system after it expires
    if (ps.life > ps.particleSystemLifetime && ps.num_alive <= 0) {
      printf("killed particle system\n");
      ps.particles_alive.clear();
      ps.particles_position.clear();
      ps.particles_velocity.clear();
      ps.particles_size.clear();
      ps.particles_life.clear();
      registry->remove_all_components_of(entity);
    }
  }

  handle_collisions();

  return true;
}

// Reset the world state to its initial state
void BoardWorldSystem::restart_game() {
  if (registry->camera.size() == 0) {
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

  // reset the game. This will reset all the mini games
  createBackground(registry, renderer);
  board = createBoard(registry, renderer, {100, 200});
  this->_generateSpaces();

  board_state = BOARD_STATE::WAITING_CONFIRMATION;

  // Generate the first player
  current_player = createDoge(registry, renderer, PLAYER_START_POS);
  registry->activePlayer.clear();
  registry->activePlayer.emplace(current_player);

  PlayerBoardMovement &pbm = registry->playerBoardMovements.get(current_player);
  pbm.target_space = board_start;
  pbm.current_space = board_start;
  pbm.roll_count_left = 0;

  // UI Creation (will be abstracted away)
  // helpTooltip =
  //    createUIelement(registry, renderer, {0.36, 0.33}, {250, 250},
  //                    TEXTURE_ASSET_ID::HELP_MAINBOARD, 1, 2, 0, 0, 0.0f);
  dice_info = createUIelement(registry, renderer, {0, -0.25}, {150, 150},
                              TEXTURE_ASSET_ID::DICE, 2, 21, 0, 0, 0);

  p1_info_box = createUIelement(registry, renderer, {-0.4, -0.4}, {200, 100},
                                TEXTURE_ASSET_ID::PLAYER_INFO, 1, 4, 0, 0, 0);
  p2_info_box = createUIelement(registry, renderer, {0.4, -0.4}, {200, 100},
                                TEXTURE_ASSET_ID::PLAYER_INFO, 1, 4, 0, 1, 0);
  p3_info_box = createUIelement(registry, renderer, {-0.4, 0.4}, {200, 100},
                                TEXTURE_ASSET_ID::PLAYER_INFO, 1, 4, 0, 2, 0);
  p4_info_box = createUIelement(registry, renderer, {0.4, 0.4}, {200, 100},
                                TEXTURE_ASSET_ID::PLAYER_INFO, 1, 4, 0, 3, 0);
  registry->UIpasses.emplace(p1_info_box);
  registry->UIpasses.emplace(p2_info_box);
  registry->UIpasses.emplace(p3_info_box);
  registry->UIpasses.emplace(p4_info_box);

  p1_standing = createUIelement(registry, renderer, {-0.443, -0.4}, {80, 80},
                                TEXTURE_ASSET_ID::RANKINGS, 1, 4, 0, 0, 0);
  p2_standing = createUIelement(registry, renderer, {0.357, -0.4}, {80, 80},
                                TEXTURE_ASSET_ID::RANKINGS, 1, 4, 0, 0, 0);
  p3_standing = createUIelement(registry, renderer, {-0.443, 0.4}, {80, 80},
                                TEXTURE_ASSET_ID::RANKINGS, 1, 4, 0, 0, 0);
  p4_standing = createUIelement(registry, renderer, {0.357, 0.4}, {80, 80},
                                TEXTURE_ASSET_ID::RANKINGS, 1, 4, 0, 0, 0);
  registry->UIpasses.emplace(p1_standing);
  registry->UIpasses.emplace(p2_standing);
  registry->UIpasses.emplace(p3_standing);
  registry->UIpasses.emplace(p4_standing);

  item_1_card = createUIelement(registry, renderer, {-0.2, 0.0}, {200, 250},
                                TEXTURE_ASSET_ID::ITEMCARDS, 1, 3, 0, 0, 0);
  item_2_card = createUIelement(registry, renderer, {0.0, 0.0}, {200, 250},
                                TEXTURE_ASSET_ID::ITEMCARDS, 1, 3, 0, 1, 0);
  item_3_card = createUIelement(registry, renderer, {0.2, 0.0}, {200, 250},
                                TEXTURE_ASSET_ID::ITEMCARDS, 1, 3, 0, 2, 0);
  registry->UIpasses.emplace(item_1_card);
  registry->UIpasses.emplace(item_2_card);
  registry->UIpasses.emplace(item_3_card);
  item_1_display =
      createUIelement(registry, renderer, {-0.2, -0.03}, {160, 160},
                      TEXTURE_ASSET_ID::ITEMS, 1, 5, 0, 0, 0);
  item_2_display = createUIelement(registry, renderer, {0.0, -0.03}, {160, 160},
                                   TEXTURE_ASSET_ID::ITEMS, 1, 5, 0, 0, 0);
  item_3_display = createUIelement(registry, renderer, {0.2, -0.03}, {160, 160},
                                   TEXTURE_ASSET_ID::ITEMS, 1, 5, 0, 0, 0);
  registry->UIpasses.emplace(item_1_display);
  registry->UIpasses.emplace(item_2_display);
  registry->UIpasses.emplace(item_3_display);
  // Control Text HELP
  confirm_text = createUIelement(registry, renderer, {0, 0.15}, {480, 60},
                                 TEXTURE_ASSET_ID::TEXT, 8, 1, 0, 0, 0);
  back_text = createUIelement(registry, renderer, {0, 0.25}, {480, 60},
                              TEXTURE_ASSET_ID::TEXT, 8, 1, 1, 0, 0);
  dice_text = createUIelement(registry, renderer, {0, 0.15}, {480, 60},
                              TEXTURE_ASSET_ID::TEXT, 8, 1, 2, 0, 0);
  item_text = createUIelement(registry, renderer, {0, 0.25}, {480, 60},
                              TEXTURE_ASSET_ID::TEXT, 8, 1, 3, 0, 0);
  registry->UIpasses.emplace(confirm_text);
  registry->UIpasses.emplace(back_text);
  registry->UIpasses.emplace(dice_text);
  registry->UIpasses.emplace(item_text);
  save_text = createUIelement(registry, renderer, {0, 0.35}, {480, 60},
                              TEXTURE_ASSET_ID::TEXT, 8, 1, 4, 0, 0);
  load_text = createUIelement(registry, renderer, {0, 0.45}, {480, 60},
                              TEXTURE_ASSET_ID::TEXT, 8, 1, 5, 0, 0);
  registry->UIpasses.emplace(save_text);
  registry->UIpasses.emplace(load_text);

  // randomize seed for random calls
  rng = std::default_random_engine(std::random_device()());
}

void BoardWorldSystem::handle_sprite_animation(float delta) {
  // Loop over all collisions detected by the physics system
  auto animationRegistry = &registry->spriteAnimations;
  for (uint i = 0; i < animationRegistry->components.size(); i++) {
    SpriteAnimation &anim = animationRegistry->components[i];

    if (anim.elapsed_time >= (1 / anim.animation_speed) * 1000.0f) {
      anim.frame = (anim.frame + 1) % anim.columns;
      anim.elapsed_time = 0.0f;
    }

    anim.elapsed_time += delta;
  }
}

// Should the game be over ?
bool BoardWorldSystem::is_over() const {
  auto window = window_manager->get_window();
  return bool(glfwWindowShouldClose(window));
}

// On key callback
void BoardWorldSystem::on_key(int key, int action, int mod) {
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    restart_game();
  }

  // saving and reloading (only possible during confirmation or rolling states)
  if (board_state == BOARD_STATE::WAITING_CONFIRMATION ||
      board_state == BOARD_STATE::ROLLING) {
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
      std::ofstream myfile;
      myfile.open("save.txt", std::ios::out | std::ios::trunc);
      for (auto i : registry->players.entities) {
        myfile << registry->players.get(i).points;
        vec2 tmp = registry->transforms
                       .get(registry->playerBoardMovements.get(i).current_space)
                       .position;
        myfile << " " << tmp.x << " " << tmp.y << " ";
      }

      myfile.close();
    }

    if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
      std::ifstream myfile;
      myfile.open("save.txt");
      std::stringstream buffer;
      buffer << myfile.rdbuf();

      size_t player_count = 0;
      auto load_player = registry->players.entities[player_count];
      std::regex rx(
          R"((?:^|\s)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s))");
      std::smatch m;
      std::string result = buffer.str();
      int count = 0;
      while (std::regex_search(result, m, rx)) {
        // There are 3 numbers, and we check which number we are reading at the
        // moment
        switch (count) {
          case 0: {
            registry->players.get(load_player).points = std::stoi(m[1].str());
            registry->playerBoardMovements.get(load_player).roll_count_left = 0;
            break;
          }
          case 1: {
            registry->transforms.get(load_player).position.x =
                std::stoi(m[1].str());
            break;
          }
          case 2: {
            registry->transforms.get(load_player).position.y =
                std::stoi(m[1].str());
            // progress the player count, and if the players doesn't exist,
            // create it

            // set the loaded player's space
            for (auto i : registry->spaces.entities) {
              if (registry->transforms.get(i).position ==
                  registry->transforms.get(load_player).position) {
                // make sure player don't move.
                registry->playerBoardMovements.get(load_player).current_space =
                    i;
                registry->playerBoardMovements.get(load_player).target_space =
                    i;
              }
            }

            if (m.suffix().str() != " ") {
              std::cout << "add player weee" << std::endl;
              count = -1;
              player_count++;
              if (player_count >= registry->players.entities.size()) {
                load_player = createDoge(registry, renderer, PLAYER_START_POS);
              } else {
                load_player = registry->players.entities[player_count];
              }
            } else {
              std::cout << "end of file" << std::endl;
            }
            break;
          }
          default: {
            std::cerr << "WTF" << std::endl;
          }
        }
        result = m.suffix().str();
        count++;
      }
    }
  }

  // waiting confirmation on switching controls (Can add new players during this
  // state!)
  if (board_state == BOARD_STATE::WAITING_CONFIRMATION) {
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
      if (registry->players.entities.size() < 4) {
        // Just create a new player, to avoid overwrite the actual current
        // player
        auto temp = createDoge(registry, renderer, PLAYER_START_POS);
        (void)temp;
      } else {
        printf("4 Players Max\n");
      }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
      board_state = BOARD_STATE::ROLLING;
    }
  } else if (board_state == BOARD_STATE::PICKING_ITEM) {
    Player &player = registry->players.get(current_player);
    if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
      board_state = BOARD_STATE::ROLLING;
    }
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
      if (player.item_1 != ITEMS::NONE) {
        board_state = BOARD_STATE::PICKED_ITEM_ANIMATION;
        animation_timeout = 1000;
        printf("Using item on slot: %d\n", 1);
        use_player_item(player.item_1);
        player.item_1 = ITEMS::NONE;
      } else {
        printf("No item on that slot!\n");
        // play error sfx
        Mix_PlayChannel(1, error, 0);
      }
    }
    if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
      if (player.item_2 != ITEMS::NONE) {
        board_state = BOARD_STATE::PICKED_ITEM_ANIMATION;
        animation_timeout = 1000;
        printf("Using item on slot: %d\n", 2);
        use_player_item(player.item_2);
        player.item_2 = ITEMS::NONE;
      } else {
        printf("No item on that slot!\n");
        // play error sfx
        Mix_PlayChannel(1, error, 0);
      }
    }
    if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
      if (player.item_3 != ITEMS::NONE) {
        board_state = BOARD_STATE::PICKED_ITEM_ANIMATION;
        animation_timeout = 1000;
        printf("Using item on slot: %d\n", 3);
        use_player_item(player.item_3);
        player.item_3 = ITEMS::NONE;
      } else {
        printf("No item on that slot!\n");
        // play error sfx
        Mix_PlayChannel(1, error, 0);
      }
    }

  } else if (board_state == BOARD_STATE::PICKING_DIRECTION) {
    PlayerBoardMovement &pbm =
        registry->playerBoardMovements.get(current_player);
    Player &p = registry->players.get(current_player);
    DirectionSpace dir = registry->directionSpaces.get(pbm.current_space);
    if (key == dir.space1_key && action == GLFW_RELEASE) {
      if (dir.space1_needMini) {
        if (p.player_effect == ITEMS::MINI_MUSHROOM) {
          pbm.target_space = dir.space1;
          board_state = BOARD_STATE::MOVING;
        } else {
          Mix_PlayChannel(1, error, 0);
        }
      } else {
        pbm.target_space = dir.space1;
        board_state = BOARD_STATE::MOVING;
      }
    } else if (key == dir.space2_key && action == GLFW_RELEASE) {
      if (dir.space2_needMini) {
        if (p.player_effect == ITEMS::MINI_MUSHROOM) {
          pbm.target_space = dir.space2;
          board_state = BOARD_STATE::MOVING;
        } else {
          Mix_PlayChannel(1, error, 0);
        }
      } else {
        pbm.target_space = dir.space2;
        board_state = BOARD_STATE::MOVING;
      }
    } else if (key == dir.space3_key && action == GLFW_RELEASE) {
      if (dir.space3_needMini) {
        if (p.player_effect == ITEMS::MINI_MUSHROOM) {
          pbm.target_space = dir.space3;
          board_state = BOARD_STATE::MOVING;
        } else {
          Mix_PlayChannel(1, error, 0);
        }
      } else {
        pbm.target_space = dir.space3;
        board_state = BOARD_STATE::MOVING;
      }
    }
  } else if (board_state == BOARD_STATE::ROLLING) {
    if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
      board_state = BOARD_STATE::PICKING_ITEM;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
      int roll = rng() % (roll_max - roll_min + 1) + roll_min;
      registry->playerBoardMovements.get(current_player).roll_count_left +=
          roll;

      // reset any effects from mushrooms
      roll_max = 10;
      roll_min = 1;

      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 1;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 2;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 3;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_4 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 4;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 5;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_6 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 6;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_7 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 7;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_8 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 8;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_9 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 9;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
      registry->playerBoardMovements.get(current_player).roll_count_left += 10;
      board_state = BOARD_STATE::DICE_HIT_ANIMATION;
      Mix_PlayChannel(1, dice_hit, 0);
      animation_timeout = 300;
    } else {
      std::cout << "WARNING: Unknown command" << std::endl;
    }
  }
}

void BoardWorldSystem::on_mouse_move(vec2 mouse_position) {
  (vec2) mouse_position;  // dummy to avoid compiler warning
}

void BoardWorldSystem::give_player_random_item() {
  ITEMS random_item =
      (ITEMS)(rng() % 2 +
              1);  // %2 because we only have 2 items, + 1 to skip ITEMS::NONE
  Player &player = registry->players.get(current_player);
  if (player.item_1 == ITEMS::NONE) {
    printf("Giving item of type: %d to player on slot 1\n", int(random_item));
    player.item_1 = random_item;
  } else if (player.item_2 == ITEMS::NONE) {
    printf("Giving item of type: %d to player on slot 2\n", int(random_item));
    player.item_2 = random_item;
  } else if (player.item_3 == ITEMS::NONE) {
    printf("Giving item of type: %d to player on slot 3\n", int(random_item));
    player.item_3 = random_item;
  } else {
    printf("Player is already full on items! Discarding new item...\n");
  }
}

void BoardWorldSystem::use_player_item(ITEMS item) {
  Player &player = registry->players.get(current_player);
  (void)player;
  Mix_PlayChannel(
      1, powerup1,
      0);  // eventually will move inside if statements to play unique sfx
  if (item == ITEMS::MEGA_MUSHROOM) {
    registry->transforms.get(current_player).scale = mega_scale;
    roll_max = 20;
    roll_min = 1;
  } else if (item == ITEMS::MINI_MUSHROOM) {
    registry->transforms.get(current_player).scale = mini_scale;
    roll_max = 5;
    roll_min = 1;
  }
  player.player_effect = item;
}

void BoardWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
  auto collisionsRegistry = registry->collisions;
  for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
    // The entity and its collider
    Entity entity = collisionsRegistry.entities[i];
    Entity entity_other = collisionsRegistry.components[i].other;

    // For now, we are only interested in collisions that involve the active
    // player
    if (registry->players.has(entity) && (entity == current_player)) {
      // Checking Player - Space collisions to light up and play sound
      // TODO split into its own method ie.
      // handle_player-space_collisions(Entity player (entity), Entity space
      // (entity other))
      if (registry->players.has(entity_other)) {
        Player &player = registry->players.get(current_player);
        Player &other_player = registry->players.get(entity_other);

        if (player.player_effect == ITEMS::MEGA_MUSHROOM) {
          // we are giant and we crush other people and steal their coins
          // bug, coins are not stoler if you hit them when you enter your last
          // space (ie. effect is removed before you collide with player) adding
          // to previous bug, if a player is underneath you when you become big
          // you steal coins from them
          if (!other_player.squished) {
            Mix_PlayChannel(2, whimper, 0);
            uint stolen_coins = min(other_player.points, 6);
            player.points += stolen_coins;
            other_player.points -= stolen_coins;
            other_player.squished = 1;
          }
        }
      } else if (registry->spaces.has(entity_other)) {
        Space &space = registry->spaces.get(entity_other);
        PlayerBoardMovement &pbm =
            registry->playerBoardMovements.get(current_player);

        if (!space.player_stepped_on &&
            (pbm.current_space != pbm.target_space)) {
          if (pbm.current_space != entity_other) {
            Mix_PlayChannel(0, space_land, 0);
            pbm.roll_count_left -= space.takes_movement;
          }
          space.player_stepped_on = 1;
          pbm.current_space = entity_other;

          if (pbm.roll_count_left > 0) {
            if (space.type == SPACE_TYPE::SPACE_DIRECTION) {
              board_state = BOARD_STATE::PICKING_DIRECTION;
            } else {
              pbm.target_space = space.next_space;
            }

            printf("count: %d\n", pbm.roll_count_left);
          } else {
            // We landed on our final space. Handle coins/mushrooms/etc.
            pbm.roll_count_left = 0;  // is this needed or just fail-safe?

            printf("Landed on type: %d \n", int(space.type));
            if (space.type == SPACE_TYPE::SPACE_BLUE) {
              Mix_PlayChannel(2, bark, 1);
              Mix_PlayChannel(1, plus_coins, 0);
              registry->players.get(current_player).points += 3;
              Entity coinParticle = createParticleSystem(
                  registry, registry->transforms.get(entity_other).position,
                  {0, 900}, 300, 10, -90.0, 60, 800, 0.55, 2500, 0.25, 75, 0.25,
                  TEXTURE_ASSET_ID::COIN);
              (void)coinParticle;
              animation_timeout = 1000.0f;
            } else if (space.type == SPACE_TYPE::SPACE_RED) {
              Mix_PlayChannel(1, minus_coins, 0);
              Mix_PlayChannel(2, whimper, 0);
              registry->players.get(current_player).points =
                  max(registry->players.get(current_player).points - 3, 0);
            } else if (space.type == SPACE_TYPE::SPACE_MUSHROOM) {
              Mix_PlayChannel(1, powerup1, 0);
              give_player_random_item();
              // Handle mushroom space code here (ie. change game state to let
              // active player get random mushroom)
            } else if (space.type == SPACE_TYPE::SPACE_BOMB) {
              Mix_PlayChannel(1, explosion1, 0);
              Mix_PlayChannel(2, whimper, 0);
              registry->players.get(current_player).points =
                  max(registry->players.get(current_player).points - 10, 0);
              // Handle challenge mini-game where we deduct a random amount of
              // coins from all users all set that as the prize pool of a
              // challenge mini-game

              // Could also scrap the idea of challenge mini-games and do some
              // otehr thing like removing 10 coins from the player
            } else if (space.type == SPACE_TYPE::SPACE_SPRING) {
              // Handle player being launched to the location of another player
              // on the board
              Mix_PlayChannel(1, spring, 0);
              if (registry->playerBoardMovements.size() > 1) {
                Entity random_player =
                    registry->playerBoardMovements
                        .entities[rng() %
                                  registry->playerBoardMovements.size()];
                while (random_player == current_player) {
                  random_player =
                      registry->playerBoardMovements
                          .entities[rng() %
                                    registry->playerBoardMovements.size()];
                }
                registry->playerBoardMovements.get(current_player)
                    .current_space =
                    registry->playerBoardMovements.get(random_player)
                        .current_space;
                registry->playerBoardMovements.get(current_player)
                    .target_space =
                    registry->playerBoardMovements.get(random_player)
                        .target_space;
                registry->transforms.get(current_player).position =
                    registry->transforms.get(random_player).position;
              }

            } else if (space.type == SPACE_TYPE::SPACE_QUESTION) {
              // Happening space! In the board the happening space will let the
              // player travel to the other end of the board through a tiny
              // mini-game where the player slides along a channel filled with
              // coins. The player will be bale to adjust vertical velocity, but
              // have a constant x velocity (to reach the other end) Check out
              // the Goomba's map on MP4 for an example of this.
            } else if (space.type == SPACE_TYPE::SPACE_FORTUNE) {
              // We win a fortune!
              Mix_PlayChannel(2, bark, 3);
              Mix_PlayChannel(1, plus_coins, 0);
              registry->players.get(current_player).points += 24;
              Entity coinParticle = createParticleSystem(
                  registry, registry->transforms.get(entity_other).position,
                  {0, 500}, 400, 0.5, 90.0, 60, -550, 0.55, 2500, 0.25, 75.0f,
                  0.5, TEXTURE_ASSET_ID::COIN);
              (void)coinParticle;
              animation_timeout += 1500.0f;
            }

            board_state = BOARD_STATE::LANDED_ANIMATION;
            animation_timeout += 1000.0f;  // min time out after landing
            registry->transforms.get(current_player).scale = normal_scale;
            registry->players.get(current_player).player_effect = ITEMS::NONE;
            return;
          }
        }
      }
    }
  }
  // Remove all collisions from this simulation step
  registry->collisions.clear();
}

void BoardWorldSystem::update_active_player() {
  auto num_players = registry->players.entities.size();
  for (uint i = 0; i < num_players; i++) {
    if (registry->players.entities[i] == current_player) {
      if (i == (num_players - 1)) {
        // after finishing all players, circle back to the first player
        current_player = registry->players.entities[0];
        registry->activePlayer.clear();
        registry->activePlayer.emplace(current_player);
        // TODO handle lowering of rounds left
        on_game_end_callback_ptr();
      } else {
        // update to the next player
        current_player = registry->players.entities[++i];
        registry->activePlayer.clear();
        registry->activePlayer.emplace(current_player);
      }
      printf("current player is %d, number is %d\n", int(current_player),
             int(num_players));
      return;
    }
  }

  for (Entity entity : registry->players.entities) {
    registry->players.get(entity).squished = 0;  // reset flags
  }
}

void BoardWorldSystem::_generateSpaces() {
  board_start = createSpace(registry, renderer, {1000, 700}, 50, 5);

  // main outer ring
  Entity s01 = createSpace(registry, renderer, {950, 700}, 50, 0);
  Entity s02 = createSpace(registry, renderer, {900, 700}, 50, 0);
  Entity s03 = createSpace(registry, renderer, {850, 700}, 50, 0);
  Entity s04 = createSpace(registry, renderer, {800, 700}, 50, 2);
  Entity s05 = createSpace(registry, renderer, {750, 700}, 50, 0);
  Entity s06 = createSpace(registry, renderer, {700, 700}, 50, 0);
  Entity s07 = createSpace(registry, renderer, {600, 700}, 50, 1);
  Entity s08 = createSpace(registry, renderer, {550, 700}, 50, 0);
  Entity s09 = createSpace(registry, renderer, {500, 700}, 50, 2);
  Entity s10 = createSpace(registry, renderer, {450, 700}, 50, 0);
  Entity s11 = createSpace(registry, renderer, {400, 700}, 50, 0);
  Entity s12 = createSpace(registry, renderer, {350, 700}, 50, 1);
  Entity s13 = createSpace(registry, renderer, {300, 700}, 50, 4);
  Entity s14 = createSpace(registry, renderer, {250, 700}, 50, 0);
  Entity s15 = createSpace(registry, renderer, {200, 700}, 50, 0);
  Entity s16 = createSpace(registry, renderer, {150, 700}, 50, 2);
  Entity s17 = createSpace(registry, renderer, {100, 700}, 50, 2);
  Entity s18 = createSpace(registry, renderer, {50, 700}, 50, 0);
  Entity s19 = createSpace(registry, renderer, {50, 650}, 50, 1);
  Entity s20 = createSpace(registry, renderer, {50, 600}, 50, 3);
  Entity s21 = createSpace(registry, renderer, {50, 550}, 50, 0);
  Entity s22 = createSpace(registry, renderer, {50, 500}, 50, 0);
  Entity s23 = createSpace(registry, renderer, {50, 450}, 50, 2);
  Entity s24 = createSpace(registry, renderer, {50, 400}, 50, 1);
  Entity s25 = createSpace(registry, renderer, {50, 350}, 50, 0);
  Entity s26 = createSpace(registry, renderer, {50, 300}, 50, 0);
  Entity s27 = createSpace(registry, renderer, {50, 250}, 50, 0);
  Entity s28 = createSpace(registry, renderer, {50, 200}, 50, 1);
  Entity s29 = createSpace(registry, renderer, {50, 150}, 50, 0);
  Entity s30 = createSpace(registry, renderer, {50, 100}, 50, 5);
  Entity s31 = createSpace(registry, renderer, {100, 100}, 50, 5);
  Entity s32 = createSpace(registry, renderer, {150, 100}, 50, 4);
  Entity s33 = createSpace(registry, renderer, {200, 100}, 50, 0);
  Entity s34 = createSpace(registry, renderer, {250, 100}, 50, 0);
  Entity s35 = createSpace(registry, renderer, {300, 100}, 50, 0);
  Entity s36 = createSpace(registry, renderer, {350, 100}, 50, 0);
  Entity s37 = createSpace(registry, renderer, {400, 100}, 50, 3);
  Entity s38 = createSpace(registry, renderer, {450, 100}, 50, 1);
  Entity s39 = createSpace(registry, renderer, {500, 100}, 50, 0);
  Entity s40 = createSpace(registry, renderer, {550, 100}, 50, 0);
  Entity s41 = createSpace(registry, renderer, {600, 100}, 50, 2);
  // Entity s42 = createSpace(registry, renderer, {650, 100}, 50, 0);
  Entity s43 = createSpace(registry, renderer, {700, 100}, 50, 0);
  Entity s44 = createSpace(registry, renderer, {750, 100}, 50, 0);
  Entity s45 = createSpace(registry, renderer, {800, 100}, 50, 1);
  Entity s46 = createSpace(registry, renderer, {850, 100}, 50, 0);
  Entity s47 = createSpace(registry, renderer, {900, 100}, 50, 5);
  Entity s48 = createSpace(registry, renderer, {950, 100}, 50, 5);
  Entity s49 = createSpace(registry, renderer, {950, 150}, 50, 0);
  Entity s50 = createSpace(registry, renderer, {950, 200}, 50, 0);
  Entity s51 = createSpace(registry, renderer, {950, 250}, 50, 0);
  Entity s52 = createSpace(registry, renderer, {950, 300}, 50, 2);
  Entity s53 = createSpace(registry, renderer, {950, 350}, 50, 1);
  Entity s54 = createSpace(registry, renderer, {950, 450}, 50, 0);
  Entity s55 = createSpace(registry, renderer, {950, 500}, 50, 4);
  Entity s56 = createSpace(registry, renderer, {950, 550}, 50, 1);
  Entity s57 = createSpace(registry, renderer, {950, 600}, 50, 0);
  Entity s58 = createSpace(registry, renderer, {950, 650}, 50, 2);

  // middle of board
  Entity s59 = createSpace(registry, renderer, {650, 650}, 50, 1);
  Entity s60 = createSpace(registry, renderer, {650, 600}, 50, 0);
  Entity s61 = createSpace(registry, renderer, {650, 550}, 50, 0);
  Entity s62 = createSpace(registry, renderer, {650, 500}, 50, 2);
  Entity s63 = createSpace(registry, renderer, {650, 450}, 50, 0);
  Entity s64 = createSpace(registry, renderer, {650, 400}, 50, 0);
  Entity s65 = createSpace(registry, renderer, {600, 400}, 50, 0);
  Entity s66 = createSpace(registry, renderer, {550, 400}, 50, 4);
  Entity s67 = createSpace(registry, renderer, {500, 400}, 50, 1);
  Entity s68 = createSpace(registry, renderer, {450, 400}, 50, 2);
  Entity s69 = createSpace(registry, renderer, {400, 400}, 50, 2);
  Entity s70 = createSpace(registry, renderer, {300, 400}, 50, 1);
  Entity s71 = createSpace(registry, renderer, {250, 400}, 50, 0);
  Entity s72 = createSpace(registry, renderer, {200, 400}, 50, 3);
  Entity s73 = createSpace(registry, renderer, {150, 400}, 50, 0);
  Entity s74 = createSpace(registry, renderer, {100, 400}, 50, 0);

  Entity s75 = createSpace(registry, renderer, {350, 450}, 50, 0);
  Entity s76 = createSpace(registry, renderer, {350, 500}, 50, 0);
  Entity s77 = createSpace(registry, renderer, {350, 550}, 50, 2);
  Entity s78 = createSpace(registry, renderer, {350, 600}, 50, 0);
  Entity s79 = createSpace(registry, renderer, {350, 650}, 50, 0);

  Entity s80 = createSpace(registry, renderer, {350, 300}, 50, 6);
  Entity s81 = createSpace(registry, renderer, {350, 250}, 50, 6);
  Entity s82 = createSpace(registry, renderer, {350, 200}, 50, 3);
  Entity s83 = createSpace(registry, renderer, {350, 150}, 50, 0);

  Entity s84 = createSpace(registry, renderer, {650, 150}, 50, 2);
  Entity s85 = createSpace(registry, renderer, {650, 200}, 50, 2);
  Entity s86 = createSpace(registry, renderer, {650, 300}, 50, 0);
  Entity s87 = createSpace(registry, renderer, {650, 350}, 50, 1);

  Entity s88 = createSpace(registry, renderer, {750, 250}, 50, 6);
  Entity s89 = createSpace(registry, renderer, {800, 250}, 50, 6);
  Entity s90 = createSpace(registry, renderer, {850, 250}, 50, 6);
  Entity s91 = createSpace(registry, renderer, {900, 250}, 50, 3);

  Entity s92 = createSpace(registry, renderer, {900, 400}, 50, 2);
  Entity s93 = createSpace(registry, renderer, {850, 400}, 50, 0);
  Entity s94 = createSpace(registry, renderer, {800, 400}, 50, 0);
  Entity s95 = createSpace(registry, renderer, {750, 400}, 50, 1);
  Entity s96 = createSpace(registry, renderer, {700, 400}, 50, 0);

  // direction spaces
  Entity d1 = createSpace(registry, renderer, {650, 700}, 50, 7);
  Space &d1s = registry->spaces.get(d1);
  d1s.takes_movement = 0;
  DirectionSpace &d1c = registry->directionSpaces.emplace(d1);
  d1c.space1 = s07;
  d1c.space1_key = GLFW_KEY_LEFT;
  d1c.space2 = s59;
  d1c.space2_key = GLFW_KEY_UP;

  Entity d2 = createSpace(registry, renderer, {350, 400}, 50, 7);
  Space &d2s = registry->spaces.get(d2);
  d2s.takes_movement = 0;
  DirectionSpace &d2c = registry->directionSpaces.emplace(d2);
  d2c.space1 = s70;
  d2c.space1_key = GLFW_KEY_LEFT;
  d2c.space2 = s80;
  d2c.space2_key = GLFW_KEY_UP;
  d2c.space2_needMini = 1;
  d2c.space3 = s75;
  d2c.space3_key = GLFW_KEY_DOWN;

  Entity d3 = createSpace(registry, renderer, {650, 100}, 50, 7);
  Space &d3s = registry->spaces.get(d3);
  d3s.takes_movement = 0;
  DirectionSpace &d3c = registry->directionSpaces.emplace(d3);
  d3c.space1 = s43;
  d3c.space1_key = GLFW_KEY_RIGHT;
  d3c.space2 = s84;
  d3c.space2_key = GLFW_KEY_DOWN;

  Entity d4 = createSpace(registry, renderer, {950, 400}, 50, 7);
  Space &d4s = registry->spaces.get(d4);
  d4s.takes_movement = 0;
  DirectionSpace &d4c = registry->directionSpaces.emplace(d4);
  d4c.space1 = s54;
  d4c.space1_key = GLFW_KEY_DOWN;
  d4c.space2 = s92;
  d4c.space2_key = GLFW_KEY_LEFT;

  Entity d5 = createSpace(registry, renderer, {650, 250}, 50, 7);
  Space &d5s = registry->spaces.get(d5);
  d5s.takes_movement = 0;
  DirectionSpace &d5c = registry->directionSpaces.emplace(d5);
  d5c.space1 = s88;
  d5c.space1_key = GLFW_KEY_RIGHT;
  d5c.space1_needMini = 1;
  d5c.space2 = s86;
  d5c.space2_key = GLFW_KEY_DOWN;

  Space &s00c = registry->spaces.get(board_start);
  s00c.next_space = s01;
  s00c.takes_movement = 0;
  s00c.player_stepped_on = 1;
  Space &s01c = registry->spaces.get(s01);
  s01c.next_space = s02;
  Space &s02c = registry->spaces.get(s02);
  s02c.next_space = s03;
  Space &s03c = registry->spaces.get(s03);
  s03c.next_space = s04;
  Space &s04c = registry->spaces.get(s04);
  s04c.next_space = s05;
  Space &s05c = registry->spaces.get(s05);
  s05c.next_space = s06;
  Space &s06c = registry->spaces.get(s06);
  s06c.next_space = d1;
  Space &s07c = registry->spaces.get(s07);
  s07c.next_space = s08;
  Space &s08c = registry->spaces.get(s08);
  s08c.next_space = s09;
  Space &s09c = registry->spaces.get(s09);
  s09c.next_space = s10;
  Space &s10c = registry->spaces.get(s10);
  s10c.next_space = s11;
  Space &s11c = registry->spaces.get(s11);
  s11c.next_space = s12;
  Space &s12c = registry->spaces.get(s12);
  s12c.next_space = s13;
  Space &s13c = registry->spaces.get(s13);
  s13c.next_space = s14;
  Space &s14c = registry->spaces.get(s14);
  s14c.next_space = s15;
  Space &s15c = registry->spaces.get(s15);
  s15c.next_space = s16;
  Space &s16c = registry->spaces.get(s16);
  s16c.next_space = s17;
  Space &s17c = registry->spaces.get(s17);
  s17c.next_space = s18;
  Space &s18c = registry->spaces.get(s18);
  s18c.next_space = s19;
  Space &s19c = registry->spaces.get(s19);
  s19c.next_space = s20;
  Space &s20c = registry->spaces.get(s20);
  s20c.next_space = s21;
  Space &s21c = registry->spaces.get(s21);
  s21c.next_space = s22;
  Space &s22c = registry->spaces.get(s22);
  s22c.next_space = s23;
  Space &s23c = registry->spaces.get(s23);
  s23c.next_space = s24;
  Space &s24c = registry->spaces.get(s24);
  s24c.next_space = s25;
  Space &s25c = registry->spaces.get(s25);
  s25c.next_space = s26;
  Space &s26c = registry->spaces.get(s26);
  s26c.next_space = s27;
  Space &s27c = registry->spaces.get(s27);
  s27c.next_space = s28;
  Space &s28c = registry->spaces.get(s28);
  s28c.next_space = s29;
  Space &s29c = registry->spaces.get(s29);
  s29c.next_space = s30;
  Space &s30c = registry->spaces.get(s30);
  s30c.next_space = s31;
  Space &s31c = registry->spaces.get(s31);
  s31c.next_space = s32;
  Space &s32c = registry->spaces.get(s32);
  s32c.next_space = s33;
  Space &s33c = registry->spaces.get(s33);
  s33c.next_space = s34;
  Space &s34c = registry->spaces.get(s34);
  s34c.next_space = s35;
  Space &s35c = registry->spaces.get(s35);
  s35c.next_space = s36;
  Space &s36c = registry->spaces.get(s36);
  s36c.next_space = s37;
  Space &s37c = registry->spaces.get(s37);
  s37c.next_space = s38;
  Space &s38c = registry->spaces.get(s38);
  s38c.next_space = s39;
  Space &s39c = registry->spaces.get(s39);
  s39c.next_space = s40;
  Space &s40c = registry->spaces.get(s40);
  s40c.next_space = s41;
  Space &s41c = registry->spaces.get(s41);
  s41c.next_space = d3;
  // Space &s42c = registry->spaces.get(s42);
  // s42c.next_space = s43;
  Space &s43c = registry->spaces.get(s43);
  s43c.next_space = s44;
  Space &s44c = registry->spaces.get(s44);
  s44c.next_space = s45;
  Space &s45c = registry->spaces.get(s45);
  s45c.next_space = s46;
  Space &s46c = registry->spaces.get(s46);
  s46c.next_space = s47;
  Space &s47c = registry->spaces.get(s47);
  s47c.next_space = s48;
  Space &s48c = registry->spaces.get(s48);
  s48c.next_space = s49;
  Space &s49c = registry->spaces.get(s49);
  s49c.next_space = s50;
  Space &s50c = registry->spaces.get(s50);
  s50c.next_space = s51;
  Space &s51c = registry->spaces.get(s51);
  s51c.next_space = s52;
  Space &s52c = registry->spaces.get(s52);
  s52c.next_space = s53;
  Space &s53c = registry->spaces.get(s53);
  s53c.next_space = d4;
  Space &s54c = registry->spaces.get(s54);
  s54c.next_space = s55;
  Space &s55c = registry->spaces.get(s55);
  s55c.next_space = s56;
  Space &s56c = registry->spaces.get(s56);
  s56c.next_space = s57;
  Space &s57c = registry->spaces.get(s57);
  s57c.next_space = s58;
  Space &s58c = registry->spaces.get(s58);
  s58c.next_space = s01;

  // first upper arm
  Space &s59c = registry->spaces.get(s59);
  s59c.next_space = s60;
  Space &s60c = registry->spaces.get(s60);
  s60c.next_space = s61;
  Space &s61c = registry->spaces.get(s61);
  s61c.next_space = s62;
  Space &s62c = registry->spaces.get(s62);
  s62c.next_space = s63;
  Space &s63c = registry->spaces.get(s63);
  s63c.next_space = s64;

  // middle arm
  Space &s92c = registry->spaces.get(s92);
  s92c.next_space = s93;
  Space &s93c = registry->spaces.get(s93);
  s93c.next_space = s94;
  Space &s94c = registry->spaces.get(s94);
  s94c.next_space = s95;
  Space &s95c = registry->spaces.get(s95);
  s95c.next_space = s96;
  Space &s96c = registry->spaces.get(s96);
  s96c.next_space = s64;
  Space &s64c = registry->spaces.get(s64);
  s64c.next_space = s65;
  Space &s65c = registry->spaces.get(s65);
  s65c.next_space = s66;
  Space &s66c = registry->spaces.get(s66);
  s66c.next_space = s67;
  Space &s67c = registry->spaces.get(s67);
  s67c.next_space = s68;
  Space &s68c = registry->spaces.get(s68);
  s68c.next_space = s69;
  Space &s69c = registry->spaces.get(s69);
  s69c.next_space = d2;
  Space &s70c = registry->spaces.get(s70);
  s70c.next_space = s71;
  Space &s71c = registry->spaces.get(s71);
  s71c.next_space = s72;
  Space &s72c = registry->spaces.get(s72);
  s72c.next_space = s73;
  Space &s73c = registry->spaces.get(s73);
  s73c.next_space = s74;
  Space &s74c = registry->spaces.get(s74);
  s74c.next_space = s24;

  Space &s75c = registry->spaces.get(s75);
  s75c.next_space = s76;
  Space &s76c = registry->spaces.get(s76);
  s76c.next_space = s77;
  Space &s77c = registry->spaces.get(s77);
  s77c.next_space = s78;
  Space &s78c = registry->spaces.get(s78);
  s78c.next_space = s79;
  Space &s79c = registry->spaces.get(s79);
  s79c.next_space = s12;

  Space &s80c = registry->spaces.get(s80);
  s80c.next_space = s81;
  Space &s81c = registry->spaces.get(s81);
  s81c.next_space = s82;
  Space &s82c = registry->spaces.get(s82);
  s82c.next_space = s83;
  Space &s83c = registry->spaces.get(s83);
  s83c.next_space = s36;

  Space &s84c = registry->spaces.get(s84);
  s84c.next_space = s85;
  Space &s85c = registry->spaces.get(s85);
  s85c.next_space = d5;
  Space &s86c = registry->spaces.get(s86);
  s86c.next_space = s87;
  Space &s87c = registry->spaces.get(s87);
  s87c.next_space = s59;

  Space &s88c = registry->spaces.get(s88);
  s88c.next_space = s89;
  Space &s89c = registry->spaces.get(s89);
  s89c.next_space = s90;
  Space &s90c = registry->spaces.get(s90);
  s90c.next_space = s91;
  Space &s91c = registry->spaces.get(s91);
  s91c.next_space = s51;
}
