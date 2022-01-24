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
class TemplateWorldSystem {
 public:
  TemplateWorldSystem();

  // starts the game
  void init(std::shared_ptr<TemplateRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<TemplatePhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~TemplateWorldSystem();

  // Steps the game ahead by ms milliseconds
  bool step(float delta);

  // Animate sprites
  void handle_sprite_animation(float delta);

  // Should the game be over ?
  bool is_over() const;

  // Input callback functions
  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);

  // restart level, used to reset game
  void restart_game();

 private:
  // holds the scene state
  std::shared_ptr<TemplateRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  Entity current_player;
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<TemplatePhysicsSystem> physics;

  // Audio
  Mix_Music *background_music;

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};
