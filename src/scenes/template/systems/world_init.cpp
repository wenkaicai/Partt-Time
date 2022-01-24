#include "world_init.hpp"

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

Entity createPlayer(std::shared_ptr<MacRegistry> registry,
                    std::shared_ptr<RenderSystem> renderer, vec2 pos) {
  auto entity = Entity();
  //std::default_random_engine rng;
  //std::uniform_real_distribution<float> uniform_dist;
  //float screen_width = 1200;
  //float screen_height = 800;

  // Store a reference to the potentially re-used mesh object
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
  registry->meshPtrs.emplace(entity, &mesh);

  // Setting initial motion values
  TransformComponent& transform = registry->transforms.emplace(entity);

  transform.position = pos;
  transform.rotation = 0.f;
  transform.scale = mesh.original_size * 150.f;
  transform.scale.x *= -1;  // point front to the right

  Velocity& velocity = registry->velocities.emplace(entity);
  velocity.velocity = {0.f, 0.f};

  registry->players.emplace(entity);
  registry->renderRequests.insert(
      entity,
      {TEXTURE_ASSET_ID::TEXTURE_COUNT,
       EFFECT_ASSET_ID::SALMON,
       GEOMETRY_BUFFER_ID::SALMON});

  return entity;
}