#include "scene.hpp"

#include <thread>

BoardScene::BoardScene() {
  std::shared_ptr<BoardRegistry> registry = std::make_shared<BoardRegistry>();
  this->registry = registry;

  std::shared_ptr<BoardWorldSystem> world =
      std::make_shared<BoardWorldSystem>();
  this->world = world;

  std::shared_ptr<BoardPhysicsSystem> physics =
      std::make_shared<BoardPhysicsSystem>();
  this->physics = physics;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

BoardScene::~BoardScene() {
  registry = nullptr;
  world = nullptr;
  physics = nullptr;
  renderer = nullptr;
}

void BoardScene::init(std::shared_ptr<WindowManager> window_manager,
                      std::function<void(BoardRegistry)> on_scene_end) {
  this->window_manager = window_manager;
  on_scene_end_callback_ptr = on_scene_end;

  registry->clear_all_components();

  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);
  assert(renderer->init(registry, window_width, window_height,
                        window_manager->get_window()));

  //renderer->initializeGlMeshes();

  world->init(registry, renderer, physics, window_manager, [&]() { end(); });
  physics->init(registry);

  renderer->add_text_to_be_rendered({"Loading Game "}, vec2(0.38, 0.45), 1,
                                    vec3(1, 0.541, 0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.1;
  renderer->render_text_only(vec3(0.133, 0.224, 0.722));
  screen.screen_brightness = 1.0;
}

bool BoardScene::step(float delta) {
  // display the story before going into board game
  if (!displayed_story || waiting_for_continue) {
    // printf("Press S to skip the story...\n");
    const int ROW = 60;

    // display the story background block
    ScreenState &screen = registry->screenStates.components[0];
    screen.blur_partial = true;
    screen.blur_rect_position = glm::vec4(0.17, 0.18, 0.62, 0.60);
    screen.screen_brightness = 0.6;

    // add the block
    renderer->add_text_block_to_be_rendered(
        story.substr(0, count), ROW, vec2(0.20, 0.70), 0.72, vec3(1, 1, 1),
        RenderSystem::FONTS::REGULAR, 0.15);

    renderer->add_text_to_be_rendered({"Press S to skip story"},
                                      vec2(0.48, 0.33), 0.55, vec3(1, 1, 1),
                                      RenderSystem::FONTS::LIGHT, 0);
    // use typist like display
    if (count >= int(story.size())) {
      displayed_story = true;

      // display guidance text at the end of the story
      renderer->add_text_to_be_rendered({"Press H at anytime to toggle help"},
                                        vec2(0.48, 0.30), 0.55, vec3(1, 1, 1),
                                        RenderSystem::FONTS::ITALIC, 0);

      renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                        vec2(0.48, 0.25), 0.75, vec3(1, 1, 0),
                                        RenderSystem::FONTS::BOLD, 0);
    } else {
      count++;
    }

    renderer->render_text_with_background(TEXTURE_ASSET_ID::BKGD_PLANIT,
                                          vec2(600, 400), vec2(1200, 800));
    // renderer->render_text_only(vec3(0.133, 0.224, 0.722));
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  } else {
    int window_width, window_height;
    glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                           &window_height);

    // step forward systems
    world->step(delta);
    physics->step(delta, window_width, window_height);
    world->handle_sprite_animation(delta);

    // display help panel
    if (help_on) {
      renderer->add_text_to_be_rendered(help, vec2(0.8, 0.6), 0.6,
                                        vec3(1, 1, 1),
                                        RenderSystem::FONTS::ITALIC, 0);
    }
    renderer->draw();
  }

  return true;
}

void BoardScene::reset_scene() { world->restart_game(); }

void BoardScene::on_key(int key, int action, int mod) {
  if (displayed_story && waiting_for_continue && key == GLFW_KEY_ENTER &&
      action == GLFW_RELEASE) {
    waiting_for_continue = false;
    ScreenState &screen = registry->screenStates.components[0];
    screen.blur_partial = false;
    screen.screen_brightness = 1.0;
  }

  if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    waiting_for_continue = false;
    displayed_story = true;

    ScreenState &screen = registry->screenStates.components[0];
    screen.blur_partial = false;
    screen.screen_brightness = 1.0;
  }
  if (key == GLFW_KEY_H && action == GLFW_PRESS) {
    help_on = !help_on;
    if (help_on) {
      ScreenState &screen = registry->screenStates.components[0];
      screen.blur_partial = true;
      screen.blur_rect_position = glm::vec4(0.78, 0.3, 0.22, 0.35);
    } else {
      ScreenState &screen = registry->screenStates.components[0];
      screen.blur_partial = false;
    }
  }
  world->on_key(key, action, mod);
}

void BoardScene::on_mouse_move(vec2 pos) { world->on_mouse_move(pos); }

void BoardScene::end() { on_scene_end_callback_ptr(*registry); }
