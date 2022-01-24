#pragma once

// stdlib
#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include "tiny_ecs.hpp"

// a red line for debugging purposes
Entity createLine(std::shared_ptr<ConstrainedPhysicsRegistry> registry, vec2 position,
                  vec2 size);

// the player
Entity createPlayer(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos);

Entity createRope(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
                    vec2 a, vec2 b);

Entity createBackground(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer,
                    vec2 position);
