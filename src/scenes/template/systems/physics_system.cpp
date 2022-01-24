// internal
#include "physics_system.hpp"

#include "world_init.hpp"

void TemplatePhysicsSystem::init(std::shared_ptr<TemplateRegistry> registry) {
  this->registry = registry;
}

void TemplatePhysicsSystem::step(float elapsed_ms, float window_width_px,
                                 float window_height_px) {}