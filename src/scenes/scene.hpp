/**
 * @file scene.hpp
 * @author Team Doge
 * @brief
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

class Scene {
 public:
  Scene() = default;

  // releases all associated resources
  virtual ~Scene() = default;

  // starts the scene
  // virtual void init() = 0;

  // steps the scene ahead by delta (in milliseconds)
  virtual bool step(float delta) = 0;

  virtual void reset_scene() = 0;

  // input callback for mouse and key presses
  virtual void on_key(int key, int action, int mod) = 0;

  // input callback for mouse movement
  virtual void on_mouse_move(vec2 pos) = 0;

 protected:
  // ends the scene and calls the callback
  virtual void end() = 0;
};
