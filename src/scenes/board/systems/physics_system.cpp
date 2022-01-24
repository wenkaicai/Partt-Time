// internal
#include "physics_system.hpp"

#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the
// entity
vec2 get_bounding_box(const TransformComponent &transform) {
  // abs is to avoid negative scale due to the facing direction.
  return {30, 30};
  //return {abs(transform.scale.x), abs(transform.scale.y)};
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding
// boxes and sees if the center point of either object is inside the other's
// bounding-box-circle. You can surely implement a more accurate detection
bool collides(const TransformComponent &transform1,
              const TransformComponent &transform2) {
  vec2 dp = transform1.position - transform2.position;
  float dist_squared = dot(dp, dp);
  const vec2 other_bonding_box = get_bounding_box(transform1) / 2.f;
  const float other_r_squared = dot(other_bonding_box, other_bonding_box);
  const vec2 my_bonding_box = get_bounding_box(transform2) / 2.f;
  const float my_r_squared = dot(my_bonding_box, my_bonding_box);
  const float r_squared = max(other_r_squared, my_r_squared);
  if (dist_squared < r_squared) return true;
  return false;
}

void BoardPhysicsSystem::init(std::shared_ptr<BoardRegistry> registry) {
  this->registry = registry;
}

void BoardPhysicsSystem::step(float delta, float window_width,
                              float window_height) {
  // Move entities with Velocity components
  auto velocity_registry = &registry->velocities;
  for (uint i = 0; i < velocity_registry->size(); i++) {
    Entity entity = velocity_registry->entities[i];
    if (registry->transforms.has(entity)) {
      float step_seconds = 1.0f * (delta / 1000.f);
      (void)step_seconds;
      Velocity &vel = velocity_registry->components[i];
      TransformComponent &transform = registry->transforms.get(entity);

      if (registry->playerBoardMovements.has(entity)) {
        // !!! TODO: fix the corner cases
        PlayerBoardMovement &pbm = registry->playerBoardMovements.get(entity);
        vec2 target_vector =
            registry->transforms.get(pbm.target_space).position -
            transform.position;
        vel.velocity = target_vector * 30.0f;
        vel.velocity[0] = clamp(vel.velocity[0], -100.f, 100.f);
        vel.velocity[1] = clamp(vel.velocity[1], -100.f, 100.f);
      }

      transform.position += vel.velocity * step_seconds;
    }
  }

    // update velocities with accelerations
  auto acceleration_registry = &registry->acceleration;
  for (uint i = 0; i < acceleration_registry->size(); i++) {
    Entity entity = acceleration_registry->entities[i];
    if (registry->velocities.has(entity)) {
      float step_seconds = 1.0f * (delta / 1000.f);
      (void)step_seconds;
      Acceleration &acc = acceleration_registry->components[i];
      Velocity &vel = registry->velocities.get(entity);

      vel.velocity += acc.acceleration * step_seconds;
    }
  }

  // Check for collisions between all colliders entities
  ComponentContainer<Collider> &collider_container = registry->colliders;
  for (uint i = 0; i < collider_container.components.size(); i++) {
    Entity entity_i = collider_container.entities[i];
    TransformComponent &transform_i = registry->transforms.get(entity_i);
    for (uint j = 0; j < collider_container.components.size(); j++)  // i+1
    {
      if (i == j) continue;

      Entity entity_j = collider_container.entities[j];
      TransformComponent &transform_j = registry->transforms.get(entity_j);
      if (collides(transform_i, transform_j)) {
        // Create a collisions event
        // We are abusing the ECS system a bit in that we potentially insert
        // multiple collisions for the same entity
        registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        registry->collisions.emplace_with_duplicates(entity_j, entity_i);
      } else {
        if (registry->spaces.has(entity_i) && registry->players.has(entity_j) &&
            registry->activePlayer.has(entity_j))

        {
          registry->spaces.get(entity_i).player_stepped_on = 0;
        }
      }
    }
  }

  (void)window_width;
  (void)window_height;

  // debugging of bounding boxes
  ComponentContainer<TransformComponent> &transform_container =
      registry->transforms;
  if (debugging.in_debug_mode) {
    uint size_before_adding_new = (uint)transform_container.components.size();
    for (uint i = 0; i < size_before_adding_new; i++) {
      TransformComponent &transform_i = transform_container.components[i];
      Entity entity_i = transform_container.entities[i];
      (void)entity_i;
      // visualize the radius with two axis-aligned lines
      const vec2 bonding_box = get_bounding_box(transform_i);
      float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
      vec2 line_scale1 = {transform_i.scale.x / 10, 2 * radius};
      createLine(registry, transform_i.position, line_scale1);
      vec2 line_scale2 = {2 * radius, transform_i.scale.x / 10};
      createLine(registry, transform_i.position, line_scale2);
    }
  }
}