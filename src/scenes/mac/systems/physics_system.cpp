// internal
#include "physics_system.hpp"

#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the
// entity
vec2 get_bounding_box3(const TransformComponent& transform) {
  // abs is to avoid negative scale due to the facing direction.
  return {abs(transform.scale.x), abs(transform.scale.y)};
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding
// boxes and sees if the center point of either object is inside the other's
// bounding-box-circle. You can surely implement a more accurate detection
bool collides3(const TransformComponent& transform1,
               const TransformComponent& transform2) {
  vec2 dp = transform1.position - transform2.position;
  float dist_squared = dot(dp, dp);
  const vec2 other_bonding_box = get_bounding_box3(transform1) / 2.f;
  const float other_r_squared = dot(other_bonding_box, other_bonding_box);
  const vec2 my_bonding_box = get_bounding_box3(transform2) / 2.f;
  const float my_r_squared = dot(my_bonding_box, my_bonding_box);
  const float r_squared = max(other_r_squared, my_r_squared);
  if (dist_squared < r_squared)
    return true;
  return false;
}

// calculate Rock - Rock collisions
bool circlesCollide(const TransformComponent& transform1,
                    const TransformComponent& transform2) {
  float dist = sqrt(pow(transform1.position.x - transform2.position.x, 2) +
                    pow(transform1.position.y - transform2.position.y, 2));
  if (dist < 50) {
    return true;
  }
  return false;
}

// calculate Player - Rock collisions
// source:
// https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
bool circleAndRectangleCollide(const TransformComponent& playerTransform,
                               const TransformComponent& rockTransform) {
  // salmon rectangle will have width of 100, height of 50
  float circleDistanceX =
      abs(rockTransform.position.x - playerTransform.position.x);
  if (circleDistanceX > 25 + 50)
    return false; // half rect width + circle radius
  float circleDistanceY =
      abs(rockTransform.position.y - playerTransform.position.y);
  if (circleDistanceY > 25 + 50)
    return false; // half rect height + circle radius

  if (circleDistanceX <= 25)
    return true; // half rect width
  if (circleDistanceY <= 25)
    return true; // half rect height

  float cornerDistSqrd =
      pow(circleDistanceX - 25, 2) + pow(circleDistanceY - 25, 2);
  return (cornerDistSqrd <= pow(25, 2));
}

void MacPhysicsSystem::handleMeshWallCollisions(Entity e) {
  Camera& camera = registry->camera.components[0];
  float window_height_px = camera.cameraFOV[1];
  float window_width_px = camera.cameraFOV[0];
  // if (registry->deathTimers.has(e)) {
  //   return;
  // }
  Mesh* MeshPointer = registry->meshPtrs.get(e);
  ComponentContainer<TransformComponent>& transform_container =
      registry->transforms;
  TransformComponent& TransformComponent_i = registry->transforms.get(e);
  Velocity& Vel = registry->velocities.get(e);
  Transform transform;
  transform.translate(TransformComponent_i.position);
  transform.rotate(TransformComponent_i.rotation);
  transform.scale(TransformComponent_i.scale);

  // rock collision with wall
  if (!registry->players.has(e)) {
    for (size_t i = 0; i < MeshPointer->vertices.size(); i++) {
      ColoredVertex vertex = MeshPointer->vertices[i];
      vec3 pos = vertex.position;
      vec3 globalPos = transform.mat * pos;
      int globalY = globalPos.y + TransformComponent_i.position.y;
      int globalX = globalPos.x + TransformComponent_i.position.x;
      if (globalY < 0 && Vel.velocity.y <= 0) {
        if (Vel.velocity.y == 0){
          Vel.velocity.y = 100;
        } else {
          Vel.velocity.y *= -1;
        }
      }
      if (globalY > window_height_px && Vel.velocity.y >= 0) {
        if (Vel.velocity.y == 0) {
          Vel.velocity.y = -100;
        } else {
          Vel.velocity.y *= -1;
        }
      }
      if (globalX < 0 || (globalX > window_width_px && Vel.velocity.x > 0)) {
        Vel.velocity.x *= -1;
      }
    } // player collision with wall
  } else if (!registry->rocks.has(e)) {
    for (uint i = 0; i < transform_container.components.size(); i++) {
      TransformComponent &transform_i = transform_container.components[i];
      float left = abs(transform_i.scale.x / 2);
      float right = window_width_px - abs(transform_i.scale.x) / 2;
      float up = abs(transform_i.scale.y / 2);
      float down = window_height_px - abs(transform_i.scale.y) / 2;
      if (transform_i.position.x <= left) {
        if (transform_i.position.y <= up) {
          transform_i.position.y = up;
        }
        if (transform_i.position.y >= down) {
          transform_i.position.y = down;
        }
        transform_i.position.x = left;
      } else if (transform_i.position.x >= right) {
        if (transform_i.position.y <= up) {
          transform_i.position.y = up;
        }
        if (transform_i.position.y >= down) {
          transform_i.position.y = down;
        }
        transform_i.position.x = right;
      } else if (transform_i.position.y <= up) {
        transform_i.position.y = up;
      } else if (transform_i.position.y >= down) {
        transform_i.position.y = down;
      }
    }
  }
}

void MacPhysicsSystem::init(std::shared_ptr<MacRegistry> registry) {
  this->registry = registry;
}

void MacPhysicsSystem::step(float elapsed_ms, float window_width_px,
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
  ComponentContainer<TransformComponent>& transform_container =
      registry->transforms;
  for (uint i = 0; i < transform_container.components.size(); i++) {
    TransformComponent& transform_i = transform_container.components[i];
    Entity entity_i = transform_container.entities[i];
    for (uint j = 0; j < transform_container.components.size(); j++) { // i+1
      if (i == j)
        continue;

      TransformComponent& transform_j = transform_container.components[j];
      Entity entity_j = transform_container.entities[j];
      if (registry->rocks.has(entity_i) && registry->rocks.has(entity_j)) {
        // rocks are colliding, use circles to calcualte collisions
        if (circlesCollide(transform_i, transform_j)) {
          registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        }
      } else if (registry->players.has(entity_i) &&
               registry->rocks.has(entity_j)) {
        if (circleAndRectangleCollide(transform_i, transform_j)) {
          registry->collisions.emplace_with_duplicates(entity_i, entity_j);
        }
      }
      // else if (registry->rocks.has(entity_i) &&
      //           registry->players.has(entity_j)) {
      //  if (circleAndRectangleCollide(motion_j, motion_i)) {
      //    registry->collisions.emplace_with_duplicates(entity_i, entity_j);
      //  }
      //}
      //} else if (collides(motion_i, motion_j)) {
      //  // Create a collisions event
      //  // We are abusing the ECS system a bit in that we potentially insert
      //  // muliple collisions for the same entity
      //  registry->collisions.emplace_with_duplicates(entity_i, entity_j);
      //  // registry->collisions.emplace_with_duplicates(entity_j, entity_i);
      //}
    }
  }

  // handle rock - wall collisions here
  for (Entity e : registry->rocks.entities) {
    handleMeshWallCollisions(e);
  }
  // handle player - wall collisions here
  handleMeshWallCollisions(registry->players.entities[0]);

  // debugging of bounding boxes
  if (debugging.in_debug_mode) {
    uint size_before_adding_new = (uint)transform_container.components.size();
    for (uint i = 0; i < size_before_adding_new; i++) {
      TransformComponent& transform_i = transform_container.components[i];
      Entity entity_i = transform_container.entities[i];
      (void)entity_i;
      // visualize the radius with two axis-aligned lines
      const vec2 bonding_box = get_bounding_box3(transform_i);
      float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
      vec2 line_scale1 = {transform_i.scale.x / 10, 2 * radius};
      createLine(registry, transform_i.position, line_scale1);
      vec2 line_scale2 = {2 * radius, transform_i.scale.x / 10};
      createLine(registry, transform_i.position, line_scale2);
    }
  }
}