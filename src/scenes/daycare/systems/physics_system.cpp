// internal
#include "physics_system.hpp"

#include <iostream>

#include "world_init.hpp"

void DaycarePhysicsSystem::init(std::shared_ptr<DaycareRegistry> registry) {
  this->registry = registry;
}

void DaycarePhysicsSystem::step(float delta, float vw, float vh) {
  for (int i = 0; i < registry->velocities.size(); i++) {
    auto puppy = registry->velocities.entities[i];

    if (!registry->targets.has(puppy)) {
      continue;
    }

    auto mPuppy = &registry->velocities.components[i];
    auto speed = length(mPuppy->velocity);
    auto tPuppy = &registry->transforms.get(puppy);
    tPuppy->position += mPuppy->velocity * (delta / 1000);
    auto target = registry->targets.get(puppy);

    float dist = length(target.target - tPuppy->position);

    if (dist < 50) {
      registry->targets.remove(puppy);
      mPuppy->velocity = vec2(0, 0);
      continue;
    }

    vec2 newDir = normalize(target.target - tPuppy->position);
    mPuppy->velocity = speed * newDir;
  }
}