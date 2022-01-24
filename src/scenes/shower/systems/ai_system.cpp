// internal
#include "ai_system.hpp"

#include "world_init.hpp"

void ShowerAISystem::init(std::shared_ptr<ShowerRegistry> registry) {
  this->registry = registry;
}

void ShowerAISystem::step() {
  Entity player = registry->players.entities[0];
  TransformComponent& player_transform = registry->transforms.get(player);
  TransformComponent& enemy_transform = registry->transforms.get(registry->enemyAi.entities[0]);
  Velocity& enemy_vel = registry->velocities.get(registry->enemyAi.entities[0]);

  float dist;
  dist = player_transform.position.x - enemy_transform.position.x;
  if (dist > 0) {
    enemy_vel.velocity.x = 200;
  }
  if (dist == 0) {
    enemy_vel.velocity.x = 0;
  }
  if (dist < 0) {
    enemy_vel.velocity.x = -200;
  }
  
}
