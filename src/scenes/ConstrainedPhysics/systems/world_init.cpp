#include "world_init.hpp"

Entity createLine(std::shared_ptr<ConstrainedPhysicsRegistry> registry, vec2 position,
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

  Velocity& vel = registry->velocities.emplace(entity);

  registry->debugComponents.emplace(entity);
  return entity;
}

Entity createRope(std::shared_ptr<ConstrainedPhysicsRegistry> registry, vec2 a, vec2 b) {
  Entity entity = Entity();

  // Store a reference to the potentially re-used mesh object (the value is
  // stored in the resource cache)
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::PEBBLE,
               GEOMETRY_BUFFER_ID::DEBUG_LINE});

  // Create motion
  TransformComponent& transform = registry->transforms.emplace(entity);
  float diffY = b.y - a.y;
  float diffX = b.x - a.x;
  transform.rotation = atan2(diffY, diffX);
  transform.position = {(a.x + b.x) / 2, (a.y + b.y) / 2};
  transform.scale = {sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)), 10};

  ConstrainedPhysicsRegistry::Rope& rope = registry->ropes.emplace(entity);
  rope.a = a;
  rope.b = b;
  rope.angle = transform.rotation;

  registry->debugComponents.emplace(entity);
  return entity;
}

Entity createPlayer(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();
  //std::default_random_engine rng;
  //std::uniform_real_distribution<float> uniform_dist;
  //float screen_width = 1200;
  //float screen_height = 800;

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);

  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = mesh.original_size * 100.f;
  transform.scale.x *= 1;  // point front to the right

  Velocity& velocity = registry->velocities.emplace(entity);
  velocity.velocity = {0.f, 0.f};

  registry->colors.insert(entity, {1.0,1.0,1.0});

  registry->players.emplace(entity);
  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::DOGE_CONSTRAINED,
       EFFECT_ASSET_ID::TEXTURED,
       GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}

Entity createBackground(std::shared_ptr<ConstrainedPhysicsRegistry> registry,
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
  transform.scale = vec2({1848, 1039.5});


  // Create and (empty) Cat component to be able to refer to all cats
  registry->renderRequests.insert(
      entity, {TEXTURE_ASSET_ID::BKGD_CONSTRAINED, EFFECT_ASSET_ID::TEXTURED,
               GEOMETRY_BUFFER_ID::SPRITE});

  return entity;
}