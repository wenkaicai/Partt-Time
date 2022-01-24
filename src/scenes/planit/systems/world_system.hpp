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
class PlanitWorldSystem {
 public:
  PlanitWorldSystem();

  // starts the game
  void init(std::shared_ptr<PlanitRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<PlanitPhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~PlanitWorldSystem();

  // Steps the game ahead by ms milliseconds
  bool step(float elapsed_ms);

  // Check for collisions
  void handle_collisions();
  void handle_sprite_animation(float delta);
  // Input callback functions
  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);

  // restart level
  void restart_game();

 private:
  // holds the scene state
  std::shared_ptr<PlanitRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // Number of fish eaten by the salmon, displayed in the window title
  unsigned int points;

  // Game state
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<PlanitPhysicsSystem> physics;
  float current_speed;
  float next_turtle_spawn;
  float next_fish_spawn;
  Entity player_salmon;
  Entity camera;
  // music references
  Mix_Music* background_music;
  Mix_Chunk* salmon_dead_sound;
  Mix_Chunk* salmon_eat_sound;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};
