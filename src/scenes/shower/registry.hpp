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

class ShowerRegistry : public ECSRegistry {
 public:
  // manually created list of all components this scene uses
  ComponentContainer<SoftShell> softShells;
  ComponentContainer<HardShell> hardShells;
  ComponentContainer<LightUp> lightUps;
  ComponentContainer<EnemyAi> enemyAi;
  ComponentContainer<Block> block;
  ComponentContainer<Player> players;
  ComponentContainer<Acceleration> accelerations;
  ComponentContainer<Grounded> grounded;
  ComponentContainer<Point> points;
  ComponentContainer<Bird> birds;

  // constructor that adds all containers for looping over them
  // IMPORTANT: don't forget to add any newly added containers!
  ShowerRegistry() {
    registry_list.push_back(&softShells);
    registry_list.push_back(&hardShells);
    registry_list.push_back(&debugComponents);
    registry_list.push_back(&lightUps);
    registry_list.push_back(&enemyAi);
    registry_list.push_back(&block);
    registry_list.push_back(&players);
    registry_list.push_back(&accelerations);
    registry_list.push_back(&grounded);
    registry_list.push_back(&points);
    registry_list.push_back(&birds);
  }
};
