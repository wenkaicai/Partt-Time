#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include "tiny_ecs.hpp"

// a red line for debugging purposes
Entity createLine(std::shared_ptr<DaycareRegistry> registry, vec2 position,
                  vec2 size);

// a pebble
Entity createCircle(std::shared_ptr<DaycareRegistry> registry, vec2 pos,
                    vec2 size);

Entity createBackground(std::shared_ptr<DaycareRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer, vec2 position);

Entity createGesturePath(std::shared_ptr<DaycareRegistry> registry,
                         std::shared_ptr<RenderSystem> renderer, vec2 position);

Entity createDoge(std::shared_ptr<DaycareRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos);

Entity createChewToys(std::shared_ptr<DaycareRegistry> registry,
                      std::shared_ptr<RenderSystem> renderer, vec2 pos);

Entity createFoodBowl(std::shared_ptr<DaycareRegistry> registry,
                      std::shared_ptr<RenderSystem> renderer, vec2 pos);

Entity createWaterBowl(std::shared_ptr<DaycareRegistry> registry,
                       std::shared_ptr<RenderSystem> renderer, vec2 pos);
