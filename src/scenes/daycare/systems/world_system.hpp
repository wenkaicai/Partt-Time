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
class DaycareWorldSystem {
 public:
  DaycareWorldSystem();

  // starts the game
  void init(std::shared_ptr<DaycareRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<DaycarePhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~DaycareWorldSystem();

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
  // initialize all the entities in the scene
  void initializeEntities();

  // handle collisions
  void handle_collisions();

  vec2 get_random_window_position(vec2 offset = vec2(0.f, 0.f));

  void update_bowls();

  void update_progress_bars(float delta);

  void update_debug_lines(float delta);

  void remove_gesture_path();

  // holds the scene state
  std::shared_ptr<DaycareRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  Entity current_player;
  Entity camera;
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<DaycarePhysicsSystem> physics;

  // Audio
  Mix_Music *background_music;

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};
