#pragma once

#include <memory>

#include "../registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include "tiny_ecs.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float SUSHI_BB_WIDTH = 0.4f * 170.f;
const float SUSHI_BB_HEIGHT = 0.4f * 170.f;
const float CAT_BB_WIDTH = 0.4f * 200.f;
const float CAT_BB_HEIGHT = 0.4f * 200.f;
const float BLOCK_BB_WIDTH = 0.4f * 300.f;
const float BLOCK_BB_HEIGHT = 0.4f * 300.f;
const float ENEMY_BB_WIDTH = 0.4f * 400.f;
const float ENEMY_BB_HEIGHT = 0.4f * 290.f;
const float PLAYER_BB_WIDTH = 0.4f * 200.f;
const float PLAYER_BB_HEIGHT = 0.4f * 340.f;

// the player
Entity createDoge(std::shared_ptr<ShowerRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos);
// the prey
Entity createSushi(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 position);
// the cat
Entity createCat(std::shared_ptr<ShowerRegistry> registry,
                 std::shared_ptr<RenderSystem> renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(std::shared_ptr<ShowerRegistry> registry, vec2 position,
                  vec2 size);
// Enemy
Entity creatEnemy(std::shared_ptr<ShowerRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos);
// block
Entity createBlock(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos);

Entity createBackground(std::shared_ptr<ShowerRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer, vec2 position);
Entity createCloud(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos, vec2 size);

Entity createBird(std::shared_ptr<ShowerRegistry> registry, vec2 pos,
                  vec2 size);
