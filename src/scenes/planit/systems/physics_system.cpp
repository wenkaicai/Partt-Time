// internal
#include "physics_system.hpp"

#include "world_init.hpp"

extern bool launch;

// Returns the local bounding coordinates scaled by the current size of the
// entity
vec2 get_bounding_box2(const TransformComponent& transform) {
  // abs is to avoid negative scale due to the facing direction.
  return {abs(transform.scale.x), abs(transform.scale.y)};
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding
// boxes and sees if the center point of either object is inside the other's
// bounding-box-circle. You can surely implement a more accurate detection
bool collides2(const TransformComponent& transform1,
              const TransformComponent& transform2) {
  vec2 dp = transform1.position - transform2.position;
  float dist_squared = dot(dp, dp);
  const vec2 other_bonding_box = get_bounding_box2(transform1) / 2.f;
  const float other_r_squared = dot(other_bonding_box, other_bonding_box);
  const vec2 my_bonding_box = get_bounding_box2(transform2) / 2.f;
  const float my_r_squared = dot(my_bonding_box, my_bonding_box);
  const float r_squared = max(other_r_squared, my_r_squared);
  if (dist_squared < r_squared) return true;
  return false;
}

void PlanitPhysicsSystem::init(std::shared_ptr<PlanitRegistry> registry) {
  this->registry = registry;
};

void PlanitPhysicsSystem::step(float elapsed_ms, float window_width_px,
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


  // Check for collisions between all moving entities
  ComponentContainer<TransformComponent>& transform_container = registry->transforms;
  for (uint i = 0; i < transform_container.components.size(); i++) {
    TransformComponent& transform_i = transform_container.components[i];
    Entity entity_i = transform_container.entities[i];
    for (uint j = 0; j < transform_container.components.size(); j++)  // i+1
    {
      if (i == j) continue;

      TransformComponent& transform_j = transform_container.components[j];
      if (collides2(transform_i, transform_j)) {
        Entity entity_j = transform_container.entities[j];
        // Create a collisions event
        // We are abusing the ECS system a bit in that we potentially insert
        // muliple collisions for the same entity
        registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        registry->collisions.emplace_with_duplicates(entity_j, entity_i);
      }
    }
  }

  // debugging of bounding boxes
  if (debugging.in_debug_mode) {
    uint size_before_adding_new = (uint)transform_container.components.size();
    for (uint i = 0; i < size_before_adding_new; i++) {
      TransformComponent& transform_i = transform_container.components[i];
      Entity entity_i = transform_container.entities[i];
      (void) entity_i;

      // visualize the radius with two axis-aligned lines
      const vec2 bonding_box = get_bounding_box2(transform_i);
      float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
      vec2 line_scale1 = {transform_i.scale.x / 10, 2 * radius};
      createLine(registry, transform_i.position, line_scale1);
      vec2 line_scale2 = {2 * radius, transform_i.scale.x / 10};
      createLine(registry, transform_i.position, line_scale2);
    }
  }

  // use semi-implicit euler for position/velocity calculations
  // source: https://www.youtube.com/watch?v=kxWBXd7ujx0&t=674s
  if (launch) {
    float G = 1;
    float h = elapsed_ms / 1000;
    float massPlanet = 50000;
    TransformComponent& playerTransform = registry->transforms.get(registry->players.entities[0]);
    Velocity& playerVelocity = registry->velocities.get(registry->players.entities[0]);

    vec2 center = {600, 400};
    // vec2 playerPos = playerTransform.position;

    auto gravity = [center, G, massPlanet](vec2 position) {
      vec2 toCenter = center - position;
      float r = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
      return toCenter * ((G * massPlanet) / (r * r));
    };

    vec2 xn = playerTransform.position;
    vec2 vn = playerVelocity.velocity;

    auto v = [vn, gravity](vec2 x, float h) { return vn + gravity(x) * h; };

    auto a = [xn, gravity](vec2 v, float h) {
      return gravity(xn) + gravity(xn + v * h) * h;
    };

    playerTransform.position = xn + h * v(xn, 0);
    playerVelocity.velocity = vn + h * a(vn, 0);

    playerTransform.rotation =
        atan2(playerVelocity.velocity.y, playerVelocity.velocity.x);
  }
}