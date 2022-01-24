#include "scene.hpp"

MacScene::MacScene() {
  std::shared_ptr<MacRegistry> registry = std::make_shared<MacRegistry>();
  this->registry = registry;

  std::shared_ptr<MacWorldSystem> world = std::make_shared<MacWorldSystem>();
  this->world = world;

  std::shared_ptr<MacPhysicsSystem> physics =
      std::make_shared<MacPhysicsSystem>();
  this->physics = physics;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

MacScene::~MacScene() {
  registry = nullptr;
  world = nullptr;
  physics = nullptr;
  renderer = nullptr;
}

void MacScene::init(std::shared_ptr<WindowManager> window_manager,
                    std::function<void(MacRegistry)> on_scene_end) {
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
  renderer->add_text_to_be_rendered({"Loading Game ."}, vec2(0.38, 0.45), 1,
                                    vec3(1, 0.718, 0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.2;
  renderer->render_text_only(vec3(0.133, 0.224, 0.722));
  screen.screen_brightness = 1.0;
}

bool MacScene::step(float delta) {
  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);

  // step forward systems
  world->step(delta);
  physics->step(delta, window_width, window_height);
  world->handle_collisions();
  renderer->draw();

  return true;
}

void MacScene::reset_scene() { world->restart_game(); }

void MacScene::on_key(int key, int action, int mod) {
  world->on_key(key, action, mod);
}

void MacScene::on_mouse_move(vec2 pos) { world->on_mouse_move(pos); }

void MacScene::end() { on_scene_end_callback_ptr(*registry); }
