#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class ConstrainedPhysicsSystem {
 public:
  ConstrainedPhysicsSystem() {}

  // starts the system
  void init(std::shared_ptr<ConstrainedPhysicsRegistry> registry);

  void step(float delta, float window_width, float window_height);

  vec2 get_bounding_box(const TransformComponent& transform);

  bool collides(Entity transform1, Entity transform2);

 private:
  // holds the scene state
  std::shared_ptr<ConstrainedPhysicsRegistry> registry;
};