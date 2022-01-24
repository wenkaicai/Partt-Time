#pragma once

// internal
#include "common.hpp"

// stlib
#include <random>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "../registry.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "window_manager.hpp"

// Container for all our entities and game logic. Individual rendering / update
// is deferred to the relative update() methods
class BoardWorldSystem {
 public:
  BoardWorldSystem();

  // starts the game
  void init(std::shared_ptr<BoardRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<BoardPhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~BoardWorldSystem();

  // Steps the game ahead by ms milliseconds
  bool step(float delta);

  // Animate sprites
  void handle_sprite_animation(float delta);

  // Should the game be over ?
  bool is_over() const;

  // Input callback functions
  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);
  // restart level
  void restart_game();

 private:
  void generateSpaces();

  // holds the scene state
  std::shared_ptr<BoardRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  void give_player_random_item();
  void use_player_item(ITEMS item);
  void handle_collisions();
  void update_active_player();

  Entity current_player;
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<BoardPhysicsSystem> physics;

  void _generateSpaces();
  Entity board;
  Entity camera;
  Entity board_start;
  Entity dice_info;
  // Entity helpTooltip;
  Entity p1_info_box;
  Entity p2_info_box;
  Entity p3_info_box;
  Entity p4_info_box;

  Entity p1_standing;
  Entity p2_standing;
  Entity p3_standing;
  Entity p4_standing;

  // ui positions for text
  vec2 positions[4] = {{0.14, 0.85}, {0.94, 0.85}, {0.14, 0.05}, {0.94, 0.05}};
  std::vector<Entity> standings;

  Entity item_1_card;  // TODO figure out if there is a way to keep track of
                       // UIPass component only without initializing, saves many
                       // get calls.
  Entity item_2_card;
  Entity item_3_card;
  Entity item_1_display;
  Entity item_2_display;
  Entity item_3_display;
  Entity confirm_text;
  Entity back_text;
  Entity dice_text;
  Entity item_text;
  Entity save_text;
  Entity load_text;

  int roll_min = 1;
  int roll_max = 10;
  float animation_timeout = 0.0f;  // timer to wait for animations/transitions
  vec2 normal_scale = {50, 50};
  vec2 mega_scale = {100, 100};
  vec2 mini_scale = {30, 30};
  // Audio
  Mix_Music *background_music;
  Mix_Chunk *space_land;
  Mix_Chunk *plus_coins;
  Mix_Chunk *minus_coins;
  Mix_Chunk *explosion1;
  Mix_Chunk *powerup1;
  Mix_Chunk *next_player;
  Mix_Chunk *dice_roll;
  Mix_Chunk *dice_hit;
  Mix_Chunk *error;
  Mix_Chunk *spring;
  Mix_Chunk *bark;
  Mix_Chunk *whimper;

  enum class BOARD_STATE {
    WAITING_CONFIRMATION,
    ROLLING,
    DICE_HIT_ANIMATION,
    MOVING,
    PICKING_DIRECTION,
    LANDED_ANIMATION,
    PICKING_ITEM,
    PICKED_ITEM_ANIMATION,
    GOT_ITEM_ANIMATION,
  };

  BOARD_STATE board_state;

  const glm::vec2 PLAYER_START_POS = {1000, 700};

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};
