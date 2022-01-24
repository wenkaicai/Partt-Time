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
class ConstrainedPhysicsWorldSystem {
 public:
  ConstrainedPhysicsWorldSystem();

  // starts the game
  void init(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
            std::shared_ptr<RenderSystem> renderer,
            std::shared_ptr<ConstrainedPhysicsSystem> physics,
            std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // Releases all associated resources
  ~ConstrainedPhysicsWorldSystem();

  // Steps the game ahead by ms milliseconds
  bool step(float delta);

  // Animate sprites
  void handle_sprite_animation(float delta);

  // Should the game be over ?
  bool is_over() const;

  void handle_collisions();

  // Input callback functions
  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);

  void handleVerticalSpring();
  void handleHorizontalSpring();
  void handleDiagSpring1();
  void handleDiagSpring2();

  void recalculateAngle(Entity e, vec2 a, vec2 b);
  // restart level
  void restart_game();

 private:
  // holds the scene state
  std::shared_ptr<ConstrainedPhysicsRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // C++ random number generator
  std::default_random_engine rng;
  std::uniform_real_distribution<float> uniform_dist;  // number between 0..1

  Entity current_player;
  Entity rope;
  Entity testBall;

  // vertical
  Entity topBallVert;
  Entity topMiddleLinkVert;
  Entity middleBallVert;
  Entity middleBottomLinkVert;
  Entity bottomBallVert;

  // horizontal
  Entity topBallHorz;
  Entity topMiddleLinkHorz;
  Entity middleBallHorz;
  Entity middleBottomLinkHorz;
  Entity bottomBallHorz;

  // diagonal 1
  Entity topBallDiag1;
  Entity topMiddleLinkDiag1;
  Entity middleBallDiag1;
  Entity middleBottomLinkDiag1;
  Entity bottomBallDiag1;

  // diagonal 2
  Entity topBallDiag2;
  Entity topMiddleLinkDiag2;
  Entity middleBallDiag2;
  Entity middleBottomLinkDiag2;
  Entity bottomBallDiag2;

  bool deadYet = false;
  bool wonYet = false;

  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<ConstrainedPhysicsSystem> physics;

  Entity camera;

  // Audio
  Mix_Music *background_music;

  // callback called when the game ends
  std::function<void()> on_game_end_callback_ptr;
};
