#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include "tiny_ecs.hpp"

// These are hard coded to the dimensions of the entity texture
const float ROCK_BB_WIDTH = 50.0f;
const float ROCK_BB_HEIGHT = 50.0f;

// the player
Entity createPlayer(std::shared_ptr<MacRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos);
// the enemy
Entity createRock(std::shared_ptr<MacRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(std::shared_ptr<MacRegistry> registry, vec2 position,
                  vec2 size);
// bkgd
Entity createBackground(std::shared_ptr<MacRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer,
                    vec2 position);
