#pragma once

#include <memory>
#include <vector>

#include "../registry.hpp"
#include "common.hpp"

class ShowerAISystem {
 public:
  void init(std::shared_ptr<ShowerRegistry> registry);

  void step();

 private:
  // holds the scene state
  std::shared_ptr<ShowerRegistry> registry;
};
