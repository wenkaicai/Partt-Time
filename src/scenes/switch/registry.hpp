/**
 * @file registry.hpp
 * @author Team Doge
 * @brief
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

// base class
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// stdlib
#include <vector>

class SwitchRegistry : public ECSRegistry {
 public:
  // constructor that adds all containers for looping over them
  // IMPORTANT: don't forget to add any newly added containers!
  SwitchRegistry() {}
};
