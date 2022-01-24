#include "scene.hpp"

TemplateScene::TemplateScene() {
  std::shared_ptr<TemplateRegistry> registry =
      std::make_shared<TemplateRegistry>();
  this->registry = registry;

  std::shared_ptr<TemplateWorldSystem> world =
      std::make_shared<TemplateWorldSystem>();
  this->world = world;

  std::shared_ptr<TemplatePhysicsSystem> physics =
      std::make_shared<TemplatePhysicsSystem>();
  this->physics = physics;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

TemplateScene::~TemplateScene() {
  registry = nullptr;
  world = nullptr;
  physics = nullptr;
  renderer = nullptr;
}

void TemplateScene::init(std::shared_ptr<WindowManager> window_manager,
                         std::function<void(TemplateRegistry)> on_scene_end) {
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

  renderer->add_text_to_be_rendered({"loading."}, vec2(0.43, 0.45), 1,
                                    vec3(0, 0, 0), RenderSystem::FONTS::BOLD,
                                    0);
  // renderer->render_text_only(vec3(0.0f, 0.1529f, 0.349f));
}

bool TemplateScene::step(float delta) {
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

void TemplateScene::reset_scene() { world->restart_game(); }

void TemplateScene::on_key(int key, int action, int mod) {
  world->on_key(key, action, mod);
}

void TemplateScene::on_mouse_move(vec2 pos) { world->on_mouse_move(pos); }

void TemplateScene::end() { on_scene_end_callback_ptr(*registry); }
