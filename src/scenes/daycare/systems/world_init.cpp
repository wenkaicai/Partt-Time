#include "world_init.hpp"

Entity createLine(std::shared_ptr<DaycareRegistry> registry, vec2 position,
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

Entity createCircle(std::shared_ptr<DaycareRegistry> registry, vec2 pos,
                    vec2 size) {
  auto entity = Entity();

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = size;

  // Create and (empty) Salmon component to be able to refer to all turtles
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::PEBBLE,
               GEOMETRY_BUFFER_ID::PEBBLE});

  return entity;
}

Entity createBackground(std::shared_ptr<DaycareRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer, vec2 position) {
  auto entity = Entity();
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({1920, 1080});

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BKGD_DAYCARE, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createGesturePath(std::shared_ptr<DaycareRegistry> registry,
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
  transform.scale = vec2({1200, 675});

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BKGD_GESTURE, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createDoge(std::shared_ptr<DaycareRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * 100.f;

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::DOGE,  // TEXTURE_COUNT indicates that no
                                // texture is needed
       EFFECT_ASSET_ID::ANIMATED, GEOMETRY_BUFFER_ID::SPRITE});

  SpriteAnimation& anim = registry->spriteAnimations.emplace(entity);
  anim.columns = 8;
  anim.rows = 8;
  anim.animation_speed = 24.0f;
  anim.animation = 0;
  anim.frame = 0;

  return entity;
}

Entity createChewToys(std::shared_ptr<DaycareRegistry> registry,
                      std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * 30.f;

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::CHEW_TOYS,  // TEXTURE_COUNT indicates that no
                                     // texture is needed
       EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

  registry->chewToys.emplace(entity);

  return entity;
}

Entity createFoodBowl(std::shared_ptr<DaycareRegistry> registry,
                      std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * 30.f;

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::FOOD_BOWL_FULL,  // TEXTURE_COUNT indicates that no
                                          // texture is needed
       EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

  registry->foodBowls.emplace(entity);

  return entity;
}

Entity createWaterBowl(std::shared_ptr<DaycareRegistry> registry,
                       std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * 30.f;

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::WATER_BOWL_FULL,  // TEXTURE_COUNT indicates that no
                                           // texture is needed
       EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

  registry->waterBowls.emplace(entity);

  return entity;
}
