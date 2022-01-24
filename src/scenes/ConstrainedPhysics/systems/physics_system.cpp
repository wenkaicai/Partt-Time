// internal
#include "physics_system.hpp"

#include "world_init.hpp"

void ConstrainedPhysicsSystem::init(std::shared_ptr<ConstrainedPhysicsRegistry> registry) {
  this->registry = registry;
}

vec2 ConstrainedPhysicsSystem::get_bounding_box(
    const TransformComponent& transform) {
  // abs is to avoid negative scale due to the facing direction.
  return {abs(transform.scale.x), abs(transform.scale.y)};
}

bool ConstrainedPhysicsSystem::collides(Entity e1, Entity e2) {

    if ((registry->players.has(e1) && !registry->ropes.has(e2)) || (registry->players.has(e2) && !registry->ropes.has(e1))) {
    TransformComponent& transform1 = registry->transforms.get(e1);
      TransformComponent& transform2 = registry->transforms.get(e2);
      float dist = sqrt(pow(transform1.position.x - transform2.position.x, 2) + pow(transform1.position.y - transform2.position.y, 2));
      if (dist < 50) {
        return true;
      }
    }
    return false;
}

void ConstrainedPhysicsSystem::step(float elapsed_ms, float window_width_px,
                                 float window_height_px) {
  auto& velocity_registry = registry->velocities;
  for (uint i = 0; i < velocity_registry.size(); i++) {
    Velocity& velocity = velocity_registry.components[i];
    Entity entity = velocity_registry.entities[i];
    TransformComponent& transform = registry->transforms.get(entity);
    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
    transform.position += velocity.velocity * step_seconds;
  }

  // Check for collisions between all moving entities
  ComponentContainer<TransformComponent>& transform_container =
      registry->transforms;
  for (uint i = 0; i < transform_container.components.size(); i++) {
    TransformComponent& transform_i = transform_container.components[i];
    Entity entity_i = transform_container.entities[i];
    for (uint j = 0; j < transform_container.components.size(); j++)  // i+1
    {
      if (i == j) continue;

      TransformComponent& transform_j = transform_container.components[j];
      Entity entity_j = transform_container.entities[j];
      if (registry->players.has(entity_i) || registry->players.has(entity_j)) {
        if (collides(entity_i, entity_j)) {
          registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        }
      }
    }
  }
}