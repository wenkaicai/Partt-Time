// stdlib
#include <memory>
#include <random>
#include <sstream>
#include <vector>

// internal
#include "registry.hpp"
#include "render_system.hpp"
#include "window_manager.hpp"

// base class
#include "../scene.hpp"

class SwitchPlayersScene : public Scene {
 public:
  SwitchPlayersScene();

  // releases all associated resources
  ~SwitchPlayersScene();

  // starts the scene
  void init(std::shared_ptr<WindowManager> window_manager,
            std::function<void()> on_scene_end);

  // steps the scene ahead by delta (in milliseconds)
  bool step(float delta);

  // rests the scene
  void reset_scene();

  // input callback for mouse and key presses
  void on_key(int key, int action, int mod);

  // input callback for mouse movement
  void on_mouse_move(vec2 pos);

  GameMode get_next_game_mode();

  void change_next_game_mode(GameMode new_game_mode);

  void disable_render();

  void request_new_minigame();

 private:
  /**
   * ========================== Story ==========================
   * ===========================================================
   */
  int shower_story_string_count = 0;
  std::string shower_story =
      "You have been teleported to an unknown "
      "planet. The evil alien is summoning Doge's biggest enemy: \"Cats\". The "
      "cats will scratch you when they touch you, it hurts a lot and you will "
      "get injured. However, the cat's favorite food and energy \"Sushi\" will "
      "also accidentally be dropped from the sky. Try and catch as much Sushi "
      "as possible before you get scratched. Stealing sushi makes the kingdom "
      "of Cats weaker!";

  int mac_story_string_count = 0;
  std::string mac_story =
      "In this stage, you and the other players are in outer space fixing the "
      "spaceship. After fixing there are meteorites floating towards you.This "
      "is a chance to show others that you have better skills surviving.Try "
      "and dodge the meteorites as long as possible to earn more points.";

  int planit_story_string_count = 0;
  std::string planit_story =
      "There are packs of energy found beside a planet. You and other players "
      "made a decision to each grab a pack to refill the spaceship. Shooting "
      "straight towards the energy is dangerous with the bursting speed. The "
      "good news is that the planet has been detected with strong gravity. "
      "Utilize the gravity to float like a boomerang to decrease the speed, by "
      "doing this allows you to be safe!";

  int constrained_chaos_string_count = 0;
  std::string constrained_chaos_story =
      "At this stage, you and the other players have to navigate a dangerous "
      "path. In the middle of the map, there is a device throwing revolving "
      "weapons. The player has to get to the right side of the map as quickly "
      "as possible, and avoid any weapons that come at any time. The game "
      "tests not only the player's judgment but also your courage.";

  int daycare_story_count = 0;
  std::string daycare_story =
      "Doge-y Daycare is a mini game where you have to manage a \"daycare \""
      " full of puppies. These puppies need to get enough food, water and play "
      "time to feel as happy as possible. Each puppy has a meter above them "
      "that indicates how fulfilled they are in each category. Place a puppy "
      "near a food bowl (green dot), water bowl (blue dot) or chew toy (red "
      "dot) to increase fulfillment in the corresponding category. However, "
      "puppies get distracted. Don't be surprised if you find your puppies "
      "wandering off somewhere. Once the time is up, you will be awarded "
      "points based on the average happiness of your puppies.";

  /**
   * ================ Help/Tutorial text =======================
   * ===========================================================
   */
  std::vector<std::string> shower_help = {
      "Use the LEFT and RIGHT key to move", "Press the SPACE button to jump",
      "When right beside the block, you can ", "      climb on the block when ",
      "      pressing the space button."};

  std::vector<std::string> mac_help = {
      "Use the UP, DOWN, LEFT, RIGHT key to move",
      "Try to survive longer than others before you get hit!"};

  std::vector<std::string> planit_help = {"Use the mouse to choose an angle",
                                          "      to fly around the planet."};

  std::vector<std::string> constrained_chaos_help = {
      "Use arrow keys to dodge the spring", "don't get hit by it!"};

  std::vector<std::string> daycare_help = {
      "Hover over a puppy with the mouse and hold",
      "F to move the puppies around the daycare."};

  std::shared_ptr<WindowManager> window_manager;
  std::shared_ptr<RenderSystem> renderer;
  std::shared_ptr<SwitchRegistry> registry;

  std::default_random_engine rng;

  bool game_selected = false;
  bool overwrite = false;
  bool render_tutorial = true;
  GameMode next_game_to_switch_to;
  GameMode previous_mini_game;

  // callback called when the scene ends
  std::function<void()> on_scene_end_callback_ptr;

  void _mac_game_render();
  void _planit_game_render();
  void _shower_game_render();
  void _constrained_chaos_game_render();
  void _daycare_game_render();
  // ends the scene
  void end();
};
