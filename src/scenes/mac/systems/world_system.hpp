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
#include "../render_system.hpp"
#include "physics_system.hpp"
#include "window_manager.hpp"

// Container for all our entities and game logic. Individual rendering / update
// is deferred to the relative update() methods
class MacWorldSystem {
 public:
  MacWorldSystem();

  // starts the game
  void init(std::shared_ptr<MacRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<MacPhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~MacWorldSystem();

  // Steps the game ahead by ms milliseconds
  bool step(float delta);

  // Check for collisions
  void handle_collisions();

  // Input callback functions
  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);

  // restart level
  void restart_game();

 private:
  // holds the scene state
  std::shared_ptr<MacRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // Number of fish eaten by the salmon, displayed in the window title
  unsigned int points;

  // Game state
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<MacPhysicsSystem> physics;
  float current_speed;
  float next_rock_spawn;
  Entity player_salmon;
  Entity camera;

  // music references
  Mix_Music* background_music;
  Mix_Chunk* salmon_dead_sound;
  Mix_Chunk* salmon_eat_sound;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  void handleRockPlayerBounce(Entity entity, Entity entity_other);
  void handleRockBounce(Entity entity, Entity entity_other);

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};