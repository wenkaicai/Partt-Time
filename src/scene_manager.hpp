/**
 * @file window_manager.hpp
 * @author Team Doge
 * @brief
 * @version 0.1
 * @date 2021-11-01
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include <memory>
#include <random>

#include "./scenes/ConstrainedPhysics/scene.hpp"
#include "./scenes/board/registry.hpp"
#include "./scenes/board/scene.hpp"
#include "./scenes/daycare/registry.hpp"
#include "./scenes/daycare/scene.hpp"
#include "./scenes/mac/registry.hpp"
#include "./scenes/mac/scene.hpp"
#include "./scenes/planit/registry.hpp"
#include "./scenes/planit/scene.hpp"
#include "./scenes/scene.hpp"
#include "./scenes/shower/registry.hpp"
#include "./scenes/shower/scene.hpp"
#include "./scenes/switch/scene.hpp"
#include "common.hpp"
#include "window_manager.hpp"

class SceneManager {
 public:
  SceneManager();

  ~SceneManager();

  void init(std::shared_ptr<WindowManager> window_manager);

  void step_current_scene(float delta);

  bool is_quit_game();

  int rounds_left = 10;

 private:
  std::default_random_engine rng;

  std::shared_ptr<WindowManager> window_manager;

  std::shared_ptr<Scene> current_scene;
  std::shared_ptr<Scene> current_mini_game;
  std::shared_ptr<BoardScene> board_scene;
  std::shared_ptr<SwitchPlayersScene> switch_players_scene;
  std::shared_ptr<MacScene> mac_scene;
  std::shared_ptr<ShowerScene> shower_scene;
  std::shared_ptr<PlanitScene> planit_scene;
  std::shared_ptr<ConstrainedPhysicsScene> constrained_physics_scene;
  std::shared_ptr<DaycareScene> daycare_scene;

  int num_players = 0;
  int players_played = 0;
  bool quit_game = false;

  void on_key(int key, int action, int mod);
  void on_mouse_move(vec2 pos);

  void on_board_end(BoardRegistry registry);
  void on_switch_players_end();
  void on_mac_end(MacRegistry registry);
  void on_shower_end(ShowerRegistry registry);
  void on_planit_end(PlanitRegistry registry);
  void on_constrained_physics_end(ConstrainedPhysicsRegistry registry);
  void on_daycare_end(DaycareRegistry registry);
};
