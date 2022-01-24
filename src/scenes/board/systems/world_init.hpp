#pragma once

// stdlib
#include <memory>

#include "../render_system.hpp"
#include "./registry.hpp"
#include "common.hpp"
#include "tiny_ecs.hpp"

/**
 * @brief Create a ParticleSystem
 *
 * @param position
 * @param psl
 * @param sr
 * @param sa
 * @param ca
 * @param is
 * @param isr
 * @param il
 * @param ilr
 * @param ps
 * @param psr
 * @param tex
 * @return Entity
 */
Entity createParticleSystem(std::shared_ptr<BoardRegistry> registry,
                            vec2 position, vec2 particleAcceleration, float psl, float sr, float sa,
                            int ca, float is, float isr, float il, float ilr,
                            float ps, float psr, TEXTURE_ASSET_ID tex);

//Entity createParticle(std::shared_ptr<BoardRegistry> registry,
//                      std::shared_ptr<RenderSystem> renderer, vec2 position,
//                      vec2 velocity, float size, TEXTURE_ASSET_ID texture, float liftime, vec2 acceleration, ParticleSystem &system, uint index);

// a red line for debugging purposes
Entity createLine(std::shared_ptr<BoardRegistry> registry, vec2 position,
                  vec2 size);

/**
 * @brief Create a parallaxed Background object
 * @return Entity
 */
Entity createBackground(std::shared_ptr<BoardRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer);

/**
 * @brief Create a Board object
 *
 * @param pos
 * @return Entity
 */
Entity createBoard(std::shared_ptr<BoardRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos);

/**
 * @brief Create a Space object
 *
 * @param pos
 * @param scale
 * @param type
 * @return Entity
 */
Entity createSpace(std::shared_ptr<BoardRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos,
                   float scale, int type);

/**
 * @brief Create a Doge object
 *
 * @param pos
 * @return Entity
 */
Entity createDoge(std::shared_ptr<BoardRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos);

/**
 * @brief Create a UI element with a given texture sprite sheet, and default
 * animation state
 *
 * @param pos
 * @param scale
 * @param texture
 * @param spriteRows
 * @param spriteCols
 * @param animation
 * @param frame
 * @param animationSpeed
 * @return Entity
 */
Entity createUIelement(std::shared_ptr<BoardRegistry> registry,
                       std::shared_ptr<RenderSystem> renderer, vec2 pos,
                       vec2 scale, TEXTURE_ASSET_ID texture, int spriteRows,
                       int spriteCols, int animation, int frame,
                       float animationSpeed);