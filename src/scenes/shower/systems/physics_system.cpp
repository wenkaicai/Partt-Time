// internal
#include "physics_system.hpp"

#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the
// entity
vec2 get_bounding_box1(const TransformComponent& transform) {
  // abs is to avoid negative scale due to the facing direction.
  return {abs(transform.scale.x), abs(transform.scale.y)};
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding
// boxes and sees if the center point of either object is inside the other's
// bounding-box-circle. You can surely implement a more accurate detection
bool collides1(const TransformComponent& transform1,
               const TransformComponent& transform2) {
  vec2 dp = transform1.position - transform2.position;
  float dist_squared = dot(dp, dp);
  const vec2 other_bonding_box = get_bounding_box1(transform1) / 2.f;
  const float other_r_squared = dot(other_bonding_box, other_bonding_box);
  const vec2 my_bonding_box = get_bounding_box1(transform2) / 2.f;
  const float my_r_squared = dot(my_bonding_box, my_bonding_box);
  const float r_squared = max(other_r_squared, my_r_squared);
  if (dist_squared < r_squared) return true;
  return false;
}

// helper function createBox for debugging
void ShowerPhysicsSystem::createBox(vec2 position, vec2 size) {
  float scaleY = size.y * 0.05f;
  float scaleX = size.x * 0.05f;
  if (size.x >= size.y) {
    createLine(registry, position + vec2(0, size.y / 2.0),
               vec2(size.y, scaleY));
    createLine(registry, position + vec2(size.y / 2.0, 0.0),
               vec2(scaleX, size.y));

    createLine(registry, position - vec2(0, size.y / 2.0),
               vec2(size.y, scaleY));
    createLine(registry, position - vec2(size.y / 2.0, 0.0),
               vec2(scaleX, size.y));
  } else {
    createLine(registry, position + vec2(0, size.x / 2.0),
               vec2(size.x, scaleX));
    createLine(registry, position + vec2(size.x / 2.0, 0.0),
               vec2(scaleY, size.x));

    createLine(registry, position - vec2(0, size.x / 2.0),
               vec2(size.x, scaleX));
    createLine(registry, position - vec2(size.x / 2.0, 0.0),
               vec2(scaleY, size.x));
  }
}

void ShowerPhysicsSystem::init(std::shared_ptr<ShowerRegistry> registry) {
  this->registry = registry;
}

void ShowerPhysicsSystem::step(float elapsed_ms, float window_width_px,
                               float window_height_px) {
  // update position based on velocity
  auto& velocity_registry = registry->velocities;
  for (uint i = 0; i < velocity_registry.size(); i++) {
    Velocity& velocity = velocity_registry.components[i];
    Entity entity = velocity_registry.entities[i];
    TransformComponent& transform = registry->transforms.get(entity);
    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
    transform.position += velocity.velocity * step_seconds;
  }
  // update velocity based on acceleration
  // we want to do this after updating position because we want to take
  // advantage of being able to simply set vel to 0 when we are on the gound to
  // prevent sinking
  auto& acceleration_registry = registry->accelerations;
  for (uint i = 0; i < acceleration_registry.size(); i++) {
    Acceleration& acceleration = acceleration_registry.components[i];
    Entity entity = acceleration_registry.entities[i];
    Velocity& velocity = registry->velocities.get(entity);
    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
    velocity.velocity += acceleration.acceleration * step_seconds;
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
      if (collides1(transform_i, transform_j)) {
        Entity entity_j = transform_container.entities[j];
        registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        registry->collisions.emplace_with_duplicates(entity_j, entity_i);
      }
    }
  }

  // you may need the following quantities to compute wall positions
  (void)window_width_px;
  (void)window_height_px;
  for (uint i = 0; i < transform_container.entities.size(); i++) {
    Entity entity = transform_container.entities[i];
    if (registry->players.has(entity)) {
      TransformComponent& transform_i = transform_container.get(entity);
      float left = transform_i.scale.x / -2;
      float right = 1200 - abs(transform_i.scale.x) / 2;
      if (transform_i.position.x <= left) {
        transform_i.position.x = left;
      } else if (transform_i.position.x >= right) {
        transform_i.position.x = right;
      }
    }
  }

  Entity player_salmon = registry->players.entities[0];
  Entity block = registry->block.entities[0];
  TransformComponent& player_transform =
      registry->transforms.get(player_salmon);
  TransformComponent& block_transform = registry->transforms.get(block);

  Velocity& player_vel = registry->velocities.get(player_salmon);
  Acceleration& player_acc = registry->accelerations.get(player_salmon);
  if (player_transform.position.y <
      block_transform.position.y - abs(block_transform.scale.y) / 2) {
    if (player_transform.position.y > block_transform.position.y -
                                          abs(block_transform.scale.y) / 2 -
                                          abs(player_transform.scale.y) / 2 &&
        player_transform.position.x + abs(player_transform.scale.x / 2) >
            block_transform.position.x - abs(block_transform.scale.x) / 2 &&
        player_transform.position.x - abs(player_transform.scale.x / 2) <
            block_transform.position.x + abs(block_transform.scale.x) / 2) {
      if (!registry->grounded.has(player_salmon)) {
        registry->grounded.emplace(player_salmon);
        player_vel.velocity.y = 0;
        player_acc.acceleration.y = 0;
      }
      player_transform.position.y = block_transform.position.y -
                                    abs(block_transform.scale.y) / 2 -
                                    abs(player_transform.scale.y) / 2 + 0.1;
    } else {
      if (registry->grounded.has(player_salmon)) {
        registry->grounded.remove(player_salmon);
        player_acc.acceleration.y = 300;
      }
    }
  } else {
    if (player_transform.position.x + abs(player_transform.scale.x / 2) >
            block_transform.position.x - abs(block_transform.scale.x) / 2 -
                0.1 &&
        player_transform.position.x < block_transform.position.x) {
      player_transform.position.x = block_transform.position.x -
                                    abs(player_transform.scale.x) / 2 -
                                    abs(block_transform.scale.x) / 2 + 0.2;
    } else if (player_transform.position.x - abs(player_transform.scale.x / 2) <
                   block_transform.position.x +
                       abs(block_transform.scale.x) / 2 - 0.1 &&
               player_transform.position.x > block_transform.position.x) {
      player_transform.position.x = block_transform.position.x +
                                    abs(player_transform.scale.x) / 2 +
                                    abs(block_transform.scale.x) / 2 + 0.2;
    }
  }

  if (player_transform.position.y > 675 - abs(player_transform.scale.y) / 2) {
    if (!registry->grounded.has(player_salmon)) {
      registry->grounded.emplace(player_salmon);
      player_vel.velocity.y = 0;
      player_acc.acceleration.y = 0;
    }
    player_transform.position.y = 675.1 - abs(player_transform.scale.y) / 2;
  }

  // debugging of bounding boxes
  if (debugging.in_debug_mode) {
    uint size_before_adding_new = (uint)transform_container.components.size();
    for (uint i = 0; i < size_before_adding_new; i++) {
      TransformComponent& trasnform_i = transform_container.components[i];
      Entity entity_i = transform_container.entities[i];
      (void)entity_i;

      // visualize the radius with two axis-aligned lines
      const vec2 bonding_box = get_bounding_box1(trasnform_i);
      float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
      vec2 line_scale1 = {trasnform_i.scale.x / 10, 2 * radius};
      createLine(registry, trasnform_i.position, line_scale1);
      vec2 line_scale2 = {2 * radius, trasnform_i.scale.x / 10};
      createLine(registry, trasnform_i.position, line_scale2);

      // !!! TODO A2: implement debugging of bounding boxes and mesh
      createBox(trasnform_i.position, trasnform_i.scale);
    }
  }
}
