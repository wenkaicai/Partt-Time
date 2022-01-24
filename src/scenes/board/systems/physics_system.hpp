#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

class BoardPhysicsSystem {
 public:
  BoardPhysicsSystem() {}

  // starts the system
  void init(std::shared_ptr<BoardRegistry> registry);

  void step(float delta, float window_width, float window_height);

 private:
  // holds the scene state
  std::shared_ptr<BoardRegistry> registry;
};