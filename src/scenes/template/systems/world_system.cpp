// Header
#include "world_system.hpp"

#include "world_init.hpp"

// stlib
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

#include "physics_system.hpp"
#include "window_manager.hpp"

// Game configuration

TemplateWorldSystem::TemplateWorldSystem() {
  // Seeding rng with random device
  rng = std::default_random_engine(std::random_device()());
}

TemplateWorldSystem::~TemplateWorldSystem() {
  // Destroy all created components
  registry->clear_all_components();

  // clean up all the shared pointers
  renderer = nullptr;
  registry = nullptr;
  physics = nullptr;
  window_manager = nullptr;
}

void TemplateWorldSystem::init(std::shared_ptr<TemplateRegistry> registry,
                               std::shared_ptr<RenderSystem> renderer,
                               std::shared_ptr<TemplatePhysicsSystem> physics,
                               std::shared_ptr<WindowManager> window_manager,
                               std::function<void()> on_scene_end) {
  this->registry = registry;
  this->renderer = renderer;
  this->physics = physics;
  this->window_manager = window_manager;
  this->on_game_end_callback_ptr = on_scene_end;

  // Set all states to default
  restart_game();
}

// Update our game world
bool TemplateWorldSystem::step(float delta) {
  // update camera
  registry->camera.get(camera).cameraTarget =
      registry->transforms.get(current_player).position;
  // Get the screen dimensions
  auto window = window_manager->get_window();
  int screen_width, screen_height;
  glfwGetFramebufferSize(window, &screen_width, &screen_height);

  // add step functions here

  handle_collisions();

  return true;
}

// Reset the world state to its initial state
void TemplateWorldSystem::restart_game() {
  camera = Entity();
  registry->camera.emplace(camera);
  // Debugging for memory/component leaks
  registry->list_all_components();
  printf("Restarting\n");

  // Remove all entities that we created
  // All that have a motion, we could also iterate over all fish, turtles, ...
  // but that would be more cumbersome
  while (registry->transforms.entities.size() > 0)
    registry->remove_all_components_of(registry->transforms.entities.back());

  // Debugging for memory/component leaks
  registry->list_all_components();

  // randomize seed for random calls. Without this, the random is predictable
  rng = std::default_random_engine(std::random_device()());
}

void TemplateWorldSystem::handle_sprite_animation(float delta) {
  // Loop over all animations
}

bool TemplateWorldSystem::is_over() const {}

void TemplateWorldSystem::on_key(int key, int action, int mod) {
  (void)mod;
  // Resetting game
  if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
    restart_game();
  }

  // Debugging
  if (key == GLFW_KEY_D) {
    if (action == GLFW_RELEASE)
      debugging.in_debug_mode = false;
    else
      debugging.in_debug_mode = true;
  }

  // More input here
}

void TemplateWorldSystem::on_mouse_move(vec2 mouse_position) {
  (vec2) mouse_position;  // dummy to avoid compiler warning
}

void TemplateWorldSystem::handle_collisions() {
  // Loop over all collisions detected by the physics system
}
