#include "scene_manager.hpp"

#include <iostream>

SceneManager::SceneManager() {
  rng = std::default_random_engine(std::random_device()());
}

SceneManager::~SceneManager() {
  board_scene = nullptr;
  switch_players_scene = nullptr;
  mac_scene = nullptr;
  shower_scene = nullptr;
  planit_scene = nullptr;
  current_scene = nullptr;
  daycare_scene = nullptr;
}

void SceneManager::init(std::shared_ptr<WindowManager> window_manager) {
  this->window_manager = window_manager;

  using namespace std::placeholders;
  // initialize input event handlers
  std::function<void(int, int, int)> scene_manager_on_key_callback_ptr =
      std::bind(&SceneManager::on_key, this, _1, _2, _3);
  window_manager->set_on_key_callback(scene_manager_on_key_callback_ptr);

  std::function<void(glm::vec2)> scene_manager_on_mouse_move_callback_ptr =
      std::bind(&SceneManager::on_mouse_move, this, _1);
  window_manager->set_on_mouse_move_callback(
      scene_manager_on_mouse_move_callback_ptr);

  this->board_scene = std::make_shared<BoardScene>();
  board_scene->init(window_manager,
                    [&](BoardRegistry registry) { on_board_end(registry); });

  this->switch_players_scene = std::make_shared<SwitchPlayersScene>();
  switch_players_scene->init(window_manager,
                             [&]() { on_switch_players_end(); });

  this->mac_scene = std::make_shared<MacScene>();
  mac_scene->init(window_manager,
                  [&](MacRegistry registry) { on_mac_end(registry); });

  this->shower_scene = std::make_shared<ShowerScene>();
  shower_scene->init(window_manager,
                     [&](ShowerRegistry registry) { on_shower_end(registry); });

  this->planit_scene = std::make_shared<PlanitScene>();
  planit_scene->init(window_manager,
                     [&](PlanitRegistry registry) { on_planit_end(registry); });

  this->constrained_physics_scene = std::make_shared<ConstrainedPhysicsScene>();
  constrained_physics_scene->init(window_manager,
                                  [&](ConstrainedPhysicsRegistry registry) {
                                    on_constrained_physics_end(registry);
                                  });

  this->daycare_scene = std::make_shared<DaycareScene>();
  daycare_scene->init(window_manager, [&](DaycareRegistry registry) {
    on_daycare_end(registry);
  });

  this->current_scene = board_scene;
};

void SceneManager::step_current_scene(float delta) {
  current_scene->step(delta);
};

void SceneManager::on_key(int key, int action, int mod) {
  // Debugging
  if (key == GLFW_KEY_D) {
    if (action == GLFW_RELEASE) {
      debugging.in_debug_mode = !debugging.in_debug_mode;
      if (debugging.in_debug_mode) {
        printf(
            "In Debug Mode: 1-9 for mini game, 0 to main board, r for reset\n");
      }
    }
  }

  if (debugging.in_debug_mode) {
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
      current_mini_game = nullptr;
      current_scene = switch_players_scene;
      switch_players_scene->change_next_game_mode(GameMode::MAC_GAME);
    }
    if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
      current_mini_game = nullptr;
      current_scene = switch_players_scene;
      switch_players_scene->change_next_game_mode(GameMode::SHOWER_GAME);
    }
    if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
      current_mini_game = nullptr;
      current_scene = switch_players_scene;
      switch_players_scene->change_next_game_mode(GameMode::PLANIT_GAME);
    }
    if (key == GLFW_KEY_4 && action == GLFW_RELEASE) {
      current_mini_game = nullptr;
      current_scene = switch_players_scene;
      switch_players_scene->change_next_game_mode(
          GameMode::CONSTRAINED_CHAOS_GAME);
    }
    if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
      current_mini_game = nullptr;
      current_scene = switch_players_scene;
      switch_players_scene->change_next_game_mode(GameMode::DAYCARE_GAME);
    }
    if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
      current_mini_game = board_scene;
      current_scene = current_mini_game;
    }
  }

  // Resetting game
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    current_scene->reset_scene();
  }

  // quit the game
  if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
    quit_game = true;
  }

  current_scene->on_key(key, action, mod);
};

void SceneManager::on_mouse_move(vec2 pos) {
  current_scene->on_mouse_move(pos);
};

bool SceneManager::is_quit_game() { return quit_game; }

void SceneManager::on_board_end(BoardRegistry registry) {
  num_players = registry.players.size();
  players_played = 0;
  current_scene = switch_players_scene;
  switch_players_scene->reset_scene();
  switch_players_scene->request_new_minigame();
  current_mini_game = nullptr;
};

void SceneManager::on_switch_players_end() {
  if (current_mini_game == nullptr) {
    // I don't think this ever runs for some reason.
    switch (switch_players_scene->get_next_game_mode()) {
      case GameMode::MAC_GAME:
        current_mini_game = mac_scene;
        break;
      case GameMode::SHOWER_GAME:
        current_mini_game = shower_scene;
        break;
      case GameMode::PLANIT_GAME:
        current_mini_game = planit_scene;
        break;
      case GameMode::CONSTRAINED_CHAOS_GAME:
        current_mini_game = constrained_physics_scene;
        break;
      case GameMode::DAYCARE_GAME:
        current_mini_game = daycare_scene;
        break;
      default:
        assert(false);
        break;
    }
    switch_players_scene->reset_scene();
  }

  current_mini_game->reset_scene();
  current_scene = current_mini_game;
}

void SceneManager::on_mac_end(MacRegistry registry) {
  players_played++;

  if (players_played < num_players) {
    current_scene = switch_players_scene;
    switch_players_scene->change_next_game_mode(GameMode::MAC_GAME);
    switch_players_scene->disable_render();
  } else {
    current_scene = board_scene;
    current_mini_game = nullptr;
    rounds_left--;
  }
};

void SceneManager::on_shower_end(ShowerRegistry registry) {
  players_played++;

  if (players_played < num_players) {
    current_scene = switch_players_scene;
    switch_players_scene->change_next_game_mode(GameMode::SHOWER_GAME);
    switch_players_scene->disable_render();
  } else {
    current_scene = board_scene;
    current_mini_game = nullptr;
    rounds_left--;
  }
};

void SceneManager::on_planit_end(PlanitRegistry registry) {
  players_played++;

  if (players_played < num_players) {
    current_scene = switch_players_scene;
    switch_players_scene->change_next_game_mode(GameMode::PLANIT_GAME);
    switch_players_scene->disable_render();
  } else {
    current_scene = board_scene;
    current_mini_game = nullptr;
    rounds_left--;
  }
};

void SceneManager::on_constrained_physics_end(
    ConstrainedPhysicsRegistry registry) {
  players_played++;

  if (players_played < num_players) {
    current_scene = switch_players_scene;
    switch_players_scene->change_next_game_mode(
        GameMode::CONSTRAINED_CHAOS_GAME);
    switch_players_scene->disable_render();
  } else {
    current_scene = board_scene;
    current_mini_game = nullptr;
    rounds_left--;
  }
};

void SceneManager::on_daycare_end(DaycareRegistry registry) {
  players_played++;

  if (players_played < num_players) {
    current_scene = switch_players_scene;
    switch_players_scene->change_next_game_mode(GameMode::DAYCARE_GAME);
    switch_players_scene->disable_render();
  } else {
    current_scene = board_scene;
    current_mini_game = nullptr;
    rounds_left--;
  }
};
