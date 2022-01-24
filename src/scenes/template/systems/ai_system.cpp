// internal
#include "ai_system.hpp"

#include "world_init.hpp"

void TemplateAISystem::init(std::shared_ptr<TemplateRegistry> registry) {
  this->registry = registry;
}

void TemplateAISystem::step(float elapsed_ms) {}