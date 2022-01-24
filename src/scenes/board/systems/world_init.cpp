#include "world_init.hpp"

Entity createParticleSystem(std::shared_ptr<BoardRegistry> registry,
                            vec2 position, vec2 particleAcceleration, float psl,
                            float sr, float sa, int ca, float is, float isr,
                            float il, float ilr, float ps, float psr,
                            TEXTURE_ASSET_ID tex) {
  Entity entity = Entity();

  // Setting initial motion values
  TransformComponent &transform = registry->transforms.emplace(entity);
  transform.position = position;
  transform.rotation = 0.0f;
  transform.scale = {1.0, 1.0};

  ParticleSystem &particleSystem = registry->particleSystems.emplace(entity);
  particleSystem.particleSystemLifetime = psl;
  particleSystem.spawningRate = sr;
  particleSystem.spawningAngle = sa;
  particleSystem.coneAngle = ca;
  particleSystem.initialSpeed = is;
  particleSystem.speedRandomness = isr;
  particleSystem.initialLifetime = il;
  particleSystem.lifetimeRandomness = ilr;
  particleSystem.particleSize = ps;
  particleSystem.particleSizeRandomness = psr;
  particleSystem.texture = tex;
  particleSystem.particleAcceleration = particleAcceleration;

  return entity;
}

Entity createLine(std::shared_ptr<BoardRegistry> registry, vec2 position,
                  vec2 scale) {
  Entity entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::PEBBLE,
               GEOMETRY_BUFFER_ID::DEBUG_LINE});

  // Create motion
  TransformComponent &transform = registry->transforms.emplace(entity);
  transform.rotation = 0.f;
  transform.position = position;
  transform.scale = scale;

  registry->debugComponents.emplace(entity);
  return entity;
}

Entity createBackground(std::shared_ptr<BoardRegistry> registry,
                        std::shared_ptr<RenderSystem> renderer) {
  auto fixedBackground = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &fixedMesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(fixedBackground, &fixedMesh);

  registry->UIelements.emplace(fixedBackground);

  // Setting initial motion values
  TransformComponent &fixedTransform =
      registry->transforms.emplace(fixedBackground);
  fixedTransform.position = {0, 0};
  fixedTransform.rotation = 0.0f;
  fixedTransform.scale = fixedMesh.original_size * 1200.f;

  registry->renderRequests.insert(
      fixedBackground, {TEXTURE_ASSET_ID::BKGD_0, EFFECT_ASSET_ID::TEXTURED,
                        GEOMETRY_BUFFER_ID::SPRITE});

  auto dynamicBackground = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &dynamicMesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(dynamicBackground, &dynamicBackground);

  registry->UIelements.emplace(dynamicBackground);

  // Setting initial motion values
  TransformComponent &dynamicTransform =
      registry->transforms.emplace(dynamicBackground);
  dynamicTransform.position = {0, 0};
  dynamicTransform.rotation = 0.0f;
  dynamicTransform.scale = dynamicMesh.original_size * 1200.f;

  registry->renderRequests.insert(
      dynamicBackground, {TEXTURE_ASSET_ID::BKGD_1, EFFECT_ASSET_ID::PARALLAXED,
                          GEOMETRY_BUFFER_ID::SPRITE});

  return fixedBackground;
}

Entity createBoard(std::shared_ptr<BoardRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BOARD);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent &transform = registry->transforms.emplace(entity);
  transform.position = {500, 400};
  transform.rotation = 0.f;
  transform.scale = mesh.original_size * 50.f;
  transform.scale.y *= -1;  // point front to the right

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::TEXTURE_COUNT,  // TEXTURE_COUNT indicates
                                         // that no texture is needed
       EFFECT_ASSET_ID::BOARD, GEOMETRY_BUFFER_ID::BOARD});

  registry->colors.insert(entity, {0.15f, 0.15f, 0.2f});

  return entity;
}

Entity createSpace(std::shared_ptr<BoardRegistry> registry,
                   std::shared_ptr<RenderSystem> renderer, vec2 pos,
                   float scale, int type) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Initialize the position, scale, and physics components
  auto &transform = registry->transforms.emplace(entity);
  transform.rotation = 0.0f;
  transform.scale = {1.0f, 1.0f};
  transform.position = pos;

  // Setting initial values, scale is negative to make it face the opposite way
  transform.scale = vec2({scale, scale});

  Space &space = registry->spaces.emplace(entity);
  space.type = (SPACE_TYPE)type;
  registry->colliders.emplace(entity);

  registry->renderRequests.insert(
      entity, {(TEXTURE_ASSET_ID)type, EFFECT_ASSET_ID::SPACE,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createDoge(std::shared_ptr<BoardRegistry> registry,
                  std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent &transform = registry->transforms.emplace(entity);
  transform.position = pos;
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * 50.f;

  registry->velocities.emplace(entity);
  registry->colliders.emplace(entity);

  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::DOGE,  // TEXTURE_COUNT indicates that no
                                // texture is needed
       EFFECT_ASSET_ID::ANIMATED, GEOMETRY_BUFFER_ID::SPRITE});

  // check player counts
  int size = registry->players.size();
  registry->players.emplace(entity);
  Player &player = registry->players.get(entity);
  player.player_id = size + 1;

  registry->playerBoardMovements.emplace(entity);
  // start on the first space
  registry->playerBoardMovements.get(entity).current_space =
      registry->spaces.entities[0];
  registry->playerBoardMovements.get(entity).target_space =
      registry->spaces.entities[0];

  SpriteAnimation &anim = registry->spriteAnimations.emplace(entity);
  anim.columns = 8;
  anim.rows = 8;
  anim.animation_speed = 24.0f;
  anim.animation = 0;
  anim.frame = 0;

  return entity;
}

Entity createUIelement(std::shared_ptr<BoardRegistry> registry,
                       std::shared_ptr<RenderSystem> renderer, vec2 pos,
                       vec2 scale, TEXTURE_ASSET_ID texture, int spriteRows,
                       int spriteCols, int animation, int frame,
                       float animationSpeed) {
  auto entity = Entity();

  // Store a reference to the potentially re-used mesh object
  Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  UIelement &ui = registry->UIelements.emplace(entity);
  (void)ui;
  // Setting initial motion values
  TransformComponent &transform = registry->transforms.emplace(entity);
  transform.position = pos;  // will be overwritten per frame to add camera
                             // position and fov into account
  transform.rotation = 0.0f;
  transform.scale = mesh.original_size * scale;

  registry->renderRequests.insert(
      entity, {texture, EFFECT_ASSET_ID::ANIMATED, GEOMETRY_BUFFER_ID::SPRITE});

  SpriteAnimation &anim = registry->spriteAnimations.emplace(entity);
  anim.columns = spriteCols;
  anim.rows = spriteRows;
  anim.animation_speed = animationSpeed;
  anim.animation = animation;
  anim.frame = frame;

  return entity;
}
