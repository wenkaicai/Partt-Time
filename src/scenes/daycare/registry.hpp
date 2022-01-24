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

class DaycareRegistry : public ECSRegistry {
 public:
  // manually created list of all components this scene uses
  ComponentContainer<DC_Draggable> draggables;
  ComponentContainer<DC_ProgressBar> progressBars;
  ComponentContainer<DC_Fighter> fighters;
  ComponentContainer<DC_Puppy> puppies;
  ComponentContainer<DC_FoodBowl> foodBowls;
  ComponentContainer<DC_WaterBowl> waterBowls;
  ComponentContainer<DC_ChewToy> chewToys;
  ComponentContainer<DC_Obstacle> obstacles;
  ComponentContainer<DC_DebugLine> debugLines;
  ComponentContainer<DC_WanderTarget> targets;

  // constructor that adds all containers for looping over them
  // IMPORTANT: don't forget to add any newly added containers!
  DaycareRegistry() {
    registry_list.push_back(&draggables);
    registry_list.push_back(&progressBars);
    registry_list.push_back(&fighters);
    registry_list.push_back(&puppies);
    registry_list.push_back(&foodBowls);
    registry_list.push_back(&waterBowls);
    registry_list.push_back(&chewToys);
    registry_list.push_back(&obstacles);
    registry_list.push_back(&debugLines);
    registry_list.push_back(&targets);
  }
};
