#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "../render_system.hpp"
#include "common.hpp"
#include "tiny_ecs.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float TARGET_BB_WIDTH = 0.4f * 224.f;
const float TARGET_BB_HEIGHT = 0.4f * 336.f;
const float PLANET_BB_WIDTH = 0.4f * 400.f;
const float PLANET_BB_HEIGHT = 0.4f * 400.f;

// the player
Entity createPlayer(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos);
// the target
Entity createTarget(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 position);
// the planet
Entity createPlanet(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(std::shared_ptr<PlanitRegistry> registry, vec2 position,
                  vec2 size);
// bkgd
Entity createBackground(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer,
                    vec2 position);