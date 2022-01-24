/**
 * @file window_manager.hpp
 * @author Team Doge
 * @brief
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include <functional>

#include "common.hpp"

class WindowManager {
 public:
  WindowManager();

  ~WindowManager();

  GLFWwindow* create_window(int width, int height);

  void on_key(int key, int action, int mod);

  void on_mouse_move(vec2 pos);

  void set_on_key_callback(std::function<void(int, int, int)> cb);

  void set_on_mouse_move_callback(std::function<void(glm::vec2)> cb);

  GLFWwindow* get_window();

 private:
  GLFWwindow* window;
  std::function<void(int, int, int)> on_key_callback_ptr;
  std::function<void(glm::vec2)> on_mouse_move_callback_ptr;
};
