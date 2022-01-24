#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class ShowerPhysicsSystem {
 public:
  ShowerPhysicsSystem() {}

  // starts the system
  void init(std::shared_ptr<ShowerRegistry> registry);

  void step(float delta, float window_width, float window_height);

 private:
  // holds the scene state
  std::shared_ptr<ShowerRegistry> registry;

  void createBox(vec2 position, vec2 size);
};