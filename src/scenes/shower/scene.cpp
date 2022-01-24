#include "scene.hpp"

ShowerScene::ShowerScene() {
  std::shared_ptr<ShowerRegistry> registry = std::make_shared<ShowerRegistry>();
  this->registry = registry;

  std::shared_ptr<ShowerWorldSystem> world =
      std::make_shared<ShowerWorldSystem>();
  this->world = world;

  std::shared_ptr<ShowerAISystem> ai = std::make_shared<ShowerAISystem>();
  this->ai = ai;

  std::shared_ptr<ShowerPhysicsSystem> physics =
      std::make_shared<ShowerPhysicsSystem>();
  this->physics = physics;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

ShowerScene::~ShowerScene() {
  registry = nullptr;
  world = nullptr;
  physics = nullptr;
  renderer = nullptr;
}

void ShowerScene::init(std::shared_ptr<WindowManager> window_manager,
                       std::function<void(ShowerRegistry)> on_scene_end) {
  this->window_manager = window_manager;
  on_scene_end_callback_ptr = on_scene_end;

  registry->clear_all_components();

  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);

  assert(renderer->init(registry, window_width, window_height,
                        window_manager->get_window()));
  world->init(registry, renderer, physics, window_manager, [&]() { end(); });
  ai->init(registry);
  physics->init(registry);

  renderer->add_text_to_be_rendered({"Loading Game .."}, vec2(0.38, 0.45), 1,
                                    vec3(1, 0.82, 0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.4;
  renderer->render_text_only(vec3(0.133, 0.224, 0.722));
  screen.screen_brightness = 1.0;
}

bool ShowerScene::step(float delta) {
  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);

  // step forward systems
  world->step(delta);
  ai->step();
  physics->step(delta, window_width, window_height);
  world->handle_collisions();
  renderer->draw();

  return true;
}

void ShowerScene::reset_scene() { world->restart_game(); }

void ShowerScene::on_key(int key, int action, int mod) {
  world->on_key(key, action, mod);
}

void ShowerScene::on_mouse_move(vec2 pos) { world->on_mouse_move(pos); }

void ShowerScene::end() { on_scene_end_callback_ptr(*registry); }
