#include "scene.hpp"

ConstrainedPhysicsScene::ConstrainedPhysicsScene() {
  std::shared_ptr<ConstrainedPhysicsRegistry> registry =
      std::make_shared<ConstrainedPhysicsRegistry>();
  this->registry = registry;

  std::shared_ptr<ConstrainedPhysicsWorldSystem> world =
      std::make_shared<ConstrainedPhysicsWorldSystem>();
  this->world = world;

  std::shared_ptr<ConstrainedPhysicsSystem> physics =
      std::make_shared<ConstrainedPhysicsSystem>();
  this->physics = physics;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

ConstrainedPhysicsScene::~ConstrainedPhysicsScene() {
  registry = nullptr;
  world = nullptr;
  physics = nullptr;
  renderer = nullptr;
}

void ConstrainedPhysicsScene::init(
    std::shared_ptr<WindowManager> window_manager,
    std::function<void(ConstrainedPhysicsRegistry)> on_scene_end) {
  this->window_manager = window_manager;
  on_scene_end_callback_ptr = on_scene_end;

  registry->clear_all_components();

  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);
  assert(renderer->init(registry, window_width, window_height,
                        window_manager->get_window()));
  world->init(registry, renderer, physics, window_manager, [&]() { end(); });
  physics->init(registry);

  renderer->add_text_to_be_rendered({"Loading Game ...."}, vec2(0.38, 0.45), 1,
                                    vec3(1, 0.9, 0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.8;
  renderer->render_text_only(vec3(0.133, 0.224, 0.722));
  screen.screen_brightness = 1.0;
}

bool ConstrainedPhysicsScene::step(float delta) {
  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);

  // step forward systems
  world->step(delta);
  physics->step(delta, window_width, window_height);
  world->handle_sprite_animation(delta);

  renderer->draw();

  return true;
}

void ConstrainedPhysicsScene::reset_scene() { world->restart_game(); }

void ConstrainedPhysicsScene::on_key(int key, int action, int mod) {
  world->on_key(key, action, mod);
}

void ConstrainedPhysicsScene::on_mouse_move(vec2 pos) {
  world->on_mouse_move(pos);
}

void ConstrainedPhysicsScene::end() { on_scene_end_callback_ptr(*registry); }
