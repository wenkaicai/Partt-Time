#include "world_init.hpp"

Entity createDoge(std::shared_ptr<ShowerRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = {PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT};
  transform.scale.x *= -1;  // point front to the right

  registry->velocities.emplace(entity);
  registry->accelerations.emplace(entity);
  registry->points.emplace(entity);

  // Create and (empty) Salmon component to be able to refer to all turtles
  registry->players.emplace(entity);
  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::PLAYER_DOGE,  // TEXTURE_COUNT indicates that no
                                       // txture is needed
       EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createCloud(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos,
                   vec2 size) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CLOUD);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = size;
  transform.scale.y *= -1;  // point front to the right

  Velocity& vel = registry->velocities.emplace(entity);
  vel.velocity = {-100, 0};
  registry->colors.insert(entity, {1.0f, 1.0f, 1.0f});
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::CLOUD,
               GEOMETRY_BUFFER_ID::CLOUD});

  return entity;
}

Entity createSushi(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 position) {
  // Reserve en entity
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({SUSHI_BB_WIDTH, SUSHI_BB_HEIGHT});

  registry->velocities.emplace(entity);
  registry->accelerations.emplace(entity);

  // Create an (empty) Fish component to be able to refer to all fish
  registry->softShells.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::FOOD, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

// project enemy
Entity creatEnemy(std::shared_ptr<ShowerRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 position) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT});

  registry->velocities.emplace(entity);

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->hardShells.emplace(entity);
  registry->enemyAi.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::ENEMY, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

// project block
Entity createBlock(std::shared_ptr<ShowerRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 position) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the motion
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.rotation = 0.f;
  transform.position = position;

  // Setting initial values, scale is negative to make it face the opposite way
  transform.scale = vec2({BLOCK_BB_WIDTH, BLOCK_BB_HEIGHT});

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->block.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BLOCK_1, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createCat(std::shared_ptr<ShowerRegistry> registry,
                 std::shared_ptr<RenderSystem> renderer, vec2 position) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({CAT_BB_WIDTH, CAT_BB_HEIGHT});

  registry->velocities.emplace(entity);
  registry->accelerations.emplace(entity);

  // Create and (empty) Turtle component to be able to refer to all turtles
  registry->hardShells.emplace(entity);
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::CAT, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createLine(std::shared_ptr<ShowerRegistry> registry, vec2 position,
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

Entity createBackground(std::shared_ptr<ShowerRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer, vec2 position) {
  auto entity = Entity();
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = vec2({1200, 800});

  // Create and (empty) Cat component to be able to refer to all cats
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BKGD_SHOWER, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createBird(std::shared_ptr<ShowerRegistry> registry, vec2 pos,
                  vec2 size) {
  auto entity = Entity();

  registry->birds.emplace(entity);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = size;

  // Add velocity component
  Velocity velocity;
  velocity.velocity = vec2(0, 0);
  registry->velocities.insert(entity, velocity);

  // Create and (empty) Salmon component to be able to refer to all turtles
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::PEBBLE,
               GEOMETRY_BUFFER_ID::PEBBLE});

  return entity;
}