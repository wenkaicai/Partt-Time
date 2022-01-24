#include "world_init.hpp"

Entity createPlayer(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = vec2(0.4f * 416, 0.4f * 204);
  Velocity& velocity = registry->velocities.emplace(entity);
  velocity.velocity = {0, 0};

  SpriteAnimation& anim = registry->spriteAnimations.emplace(entity);
  anim.columns = 8;
  anim.rows = 8;
  anim.animation_speed = 24.0f;
  anim.animation = 2;
  anim.frame = 0;

  // Create and (empty) Salmon component to be able to refer to all turtles
  registry->players.emplace(entity);
  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::DOGE_ROCKET,
       EFFECT_ASSET_ID::TEXTURED,
      GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createTarget(std::shared_ptr<PlanitRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 position) {
  // Reserve en entity
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  auto& transform = registry->transforms.emplace(entity);
  transform.rotation = 0.f;
  transform.position = position;
  // Setting initial values, scale is negative to make it face the opposite way
  transform.scale = vec2({-TARGET_BB_WIDTH, TARGET_BB_HEIGHT});

  // Create an (empty) Fish component to be able to refer to all fish
  registry->targets.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::ENERGY_PLANIT, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createPlanet(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer,
                    vec2 position) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the motion
  auto& transform = registry->transforms.emplace(entity);
  transform.rotation = 0.f;
  transform.position = position;
  // Setting initial values, scale is negative to make it face the opposite way
  transform.scale = vec2({-PLANET_BB_WIDTH, PLANET_BB_HEIGHT});

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->planets.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::PLANET_PLANIT, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createLine(std::shared_ptr<PlanitRegistry> registry, vec2 position,
                  vec2 scale) {
  Entity entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::PEBBLE,
               GEOMETRY_BUFFER_ID::DEBUG_LINE});

  // Create motion
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.rotation = 0.f;
  transform.position = position;
  transform.scale = scale;

  registry->debugComponents.emplace(entity);
  return entity;
}

Entity createBackground(std::shared_ptr<PlanitRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer,
                    vec2 position) {
  auto entity = Entity();
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({2000, 1250});


  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BKGD_PLANIT, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}
