#include "scene.hpp"

SwitchPlayersScene::SwitchPlayersScene() {
  rng = std::default_random_engine(std::random_device()());

  std::shared_ptr<SwitchRegistry> registry = std::make_shared<SwitchRegistry>();
  this->registry = registry;

  std::shared_ptr<RenderSystem> renderer = std::make_shared<RenderSystem>();
  this->renderer = renderer;
}

SwitchPlayersScene::~SwitchPlayersScene() { renderer = nullptr; }

void SwitchPlayersScene::init(std::shared_ptr<WindowManager> window_manager,
                              std::function<void()> on_scene_end) {
  this->window_manager = window_manager;
  on_scene_end_callback_ptr = on_scene_end;

  registry->clear_all_components();

  int window_width, window_height;
  glfwGetFramebufferSize(window_manager->get_window(), &window_width,
                         &window_height);
  assert(renderer->init(registry, window_width, window_height,
                        window_manager->get_window()));

  previous_mini_game = next_game_to_switch_to;
  next_game_to_switch_to = GameMode::MAC_GAME;
  srand(time(NULL));
}

void SwitchPlayersScene::request_new_minigame() {
  overwrite = false;
  game_selected = false;
}

bool SwitchPlayersScene::step(float delta) {
  (void)delta;

  if (!game_selected && !overwrite) {
    while (next_game_to_switch_to == previous_mini_game) {
      next_game_to_switch_to = GameMode(rand() % int(GameMode::GAME_COUNT));
      printf("Current mini game is %d\n", int(next_game_to_switch_to));
      game_selected = true;
    }
    previous_mini_game = next_game_to_switch_to;
  }

  switch (next_game_to_switch_to) {
    case GameMode::MAC_GAME:
      _mac_game_render();
      break;
    case GameMode::SHOWER_GAME:
      _shower_game_render();
      break;
    case GameMode::PLANIT_GAME:
      _planit_game_render();
      break;
    case GameMode::CONSTRAINED_CHAOS_GAME:
      _constrained_chaos_game_render();
      break;
    case GameMode::DAYCARE_GAME:
      _daycare_game_render();
      break;
    default:
      // little pice of ...
      assert(false);
      break;
  }

  return true;
}
void SwitchPlayersScene::_mac_game_render() {
  renderer->add_text_to_be_rendered({"Survive in space!"}, vec2(0.02, 0.9), 1.1,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 0);
  if (render_tutorial) {
    renderer->add_text_block_to_be_rendered(
        mac_story.substr(0, mac_story_string_count), 60, vec2(0.05, 0.8), 0.86,
        vec3(1, 1, 1), RenderSystem::FONTS::REGULAR, 0.25);
    mac_story_string_count++;
  }
  renderer->add_text_to_be_rendered(mac_help, vec2(0.70, 0.5), 0.5,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::ITALIC, 0);
  renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                    vec2(0.60, 0.1), 0.9, vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.5;
  screen.blur_partial = true;
  screen.blur_rect_position = glm::vec4(0.66, 0.42, 0.4, 0.14);

  renderer->render_text_only(vec3(0.298, 0, 0.945));
  screen.blur_partial = false;
  screen.screen_brightness = 1.0;
}
void SwitchPlayersScene::_planit_game_render() {
  renderer->add_text_to_be_rendered({"Time to Refill!"}, vec2(0.02, 0.9), 1.1,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 1.0);
  if (render_tutorial) {
    renderer->add_text_block_to_be_rendered(
        planit_story.substr(0, planit_story_string_count), 60, vec2(0.05, 0.8),
        0.86, vec3(1, 1, 1), RenderSystem::FONTS::REGULAR, 0.25);
    planit_story_string_count++;
  }

  renderer->add_text_to_be_rendered(planit_help, vec2(0.77, 0.5), 0.5,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::ITALIC, 0);
  renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                    vec2(0.60, 0.1), 0.9, vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.5;
  screen.blur_partial = true;
  screen.blur_rect_position = glm::vec4(0.75, 0.43, 0.25, 0.12);

  renderer->render_text_only(vec3(0.09, 0.259, 0.714));
  screen.blur_partial = false;
  screen.screen_brightness = 1.0;
}

void SwitchPlayersScene::_shower_game_render() {
  renderer->add_text_to_be_rendered({"Steal The Energy!"}, vec2(0.02, 0.9), 1,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 1.1);
  if (render_tutorial) {
    renderer->add_text_block_to_be_rendered(
        shower_story.substr(0, shower_story_string_count), 60, vec2(0.05, 0.8),
        0.86, vec3(1, 1, 1), RenderSystem::FONTS::REGULAR, 0.25);
    shower_story_string_count++;
  }

  renderer->add_text_to_be_rendered(shower_help, vec2(0.77, 0.6), 0.5,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::ITALIC, 0);
  renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                    vec2(0.60, 0.1), 0.9, vec3(1, 1, 1),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.75;
  screen.blur_partial = true;
  screen.blur_rect_position = glm::vec4(0.75, 0.4, 0.25, 0.25);

  renderer->render_text_only(vec3(0.973, 0.561, 0.549));
  screen.blur_partial = false;
  screen.screen_brightness = 1.0;
}

void SwitchPlayersScene::_constrained_chaos_game_render() {
  renderer->add_text_to_be_rendered({"Constrained Chaos!"}, vec2(0.02, 0.9), 1,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 1.1);
  if (render_tutorial) {
    renderer->add_text_block_to_be_rendered(
        constrained_chaos_story.substr(0, constrained_chaos_string_count), 60,
        vec2(0.05, 0.8), 0.86, vec3(1, 1, 1), RenderSystem::FONTS::REGULAR,
        0.25);
    constrained_chaos_string_count++;
  }

  renderer->add_text_to_be_rendered(constrained_chaos_help, vec2(0.77, 0.6),
                                    0.5, vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::ITALIC, 0);
  renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                    vec2(0.60, 0.1), 0.9, vec3(1, 1, 1),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.75;
  screen.blur_partial = true;
  screen.blur_rect_position = glm::vec4(0.75, 0.4, 0.25, 0.25);

  renderer->render_text_only(vec3(0.898, 0.263, 0.016));
  screen.blur_partial = false;
  screen.screen_brightness = 1.0;
}

void SwitchPlayersScene::_daycare_game_render() {
  renderer->add_text_to_be_rendered({"Day Care!"}, vec2(0.02, 0.9), 1,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::BOLD, 1.1);
  if (render_tutorial) {
    renderer->add_text_block_to_be_rendered(
        daycare_story.substr(0, daycare_story_count), 60, vec2(0.05, 0.8), 0.86,
        vec3(1, 1, 1), RenderSystem::FONTS::REGULAR, 0.25);
    daycare_story_count++;
  }

  renderer->add_text_to_be_rendered(daycare_help, vec2(0.735, 0.7), 0.5,
                                    vec3(1.0, 1.0, 1.0),
                                    RenderSystem::FONTS::ITALIC, 0);
  renderer->add_text_to_be_rendered({"Press Enter to continue..."},
                                    vec2(0.60, 0.1), 0.9, vec3(1, 1, 1),
                                    RenderSystem::FONTS::BOLD, 0);

  ScreenState &screen = registry->screenStates.components[0];
  screen.screen_brightness = 0.75;
  screen.blur_partial = true;
  screen.blur_rect_position = glm::vec4(0.72, 0.6, 0.28, 0.15);

  renderer->render_text_only(vec3(0.914, 0.553, 0.961));
  screen.blur_partial = false;
  screen.screen_brightness = 1.0;
}
void SwitchPlayersScene::reset_scene() {
  game_selected = false;
  render_tutorial = true;
}

void SwitchPlayersScene::on_key(int key, int action, int mod) {
  (void)mod;
  if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
    end();
  }
}

void SwitchPlayersScene::on_mouse_move(vec2 pos) { (void)pos; }

GameMode SwitchPlayersScene::get_next_game_mode() {
  return next_game_to_switch_to;
}

void SwitchPlayersScene::change_next_game_mode(GameMode new_game_mode) {
  overwrite = true;
  next_game_to_switch_to = new_game_mode;
}

void SwitchPlayersScene::disable_render() { render_tutorial = false; }
void SwitchPlayersScene::end() { on_scene_end_callback_ptr(); }
