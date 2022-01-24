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

class PlanitRegistry : public ECSRegistry {
 public:
  // manually created list of all components this scene uses
  ComponentContainer<Player> players;
  ComponentContainer<Planet> planets;
  ComponentContainer<Target> targets;


  // constructor that adds all containers for looping over them
  // IMPORTANT: don't forget to add any newly added containers!
  PlanitRegistry() {
    registry_list.push_back(&players);
    registry_list.push_back(&planets);
    registry_list.push_back(&targets);
  }
};
