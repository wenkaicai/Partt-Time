#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class TemplateAISystem {
 public:
  TemplateAISystem() {}

  // starts the system
  void init(std::shared_ptr<TemplateRegistry> registry);

  void step(float elapsed_ms);

 private:
  // holds the scene state
  std::shared_ptr<TemplateRegistry> registry;
};