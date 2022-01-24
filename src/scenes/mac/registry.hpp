/**
 * @file registry.hpp
 * @author Team Doge
 * @brief
 * @version 0.1
 * @date 2021-10-31
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

class MacRegistry : public ECSRegistry {
 public:
  // manually created list of all components this scene uses
  ComponentContainer<Rock> rocks;
  ComponentContainer<Player> players;

  // constructor that adds all containers for looping over them
  // IMPORTANT: don't forget to add any snewly added containers!
  MacRegistry() {
    registry_list.push_back(&rocks);
    registry_list.push_back(&players);
  }
};
