// stdlib
#include <memory>
#include <random>
#include <sstream>
#include <vector>

// internal
#include "window_manager.hpp"

// base class
#include "../scene.hpp"

// registry
#include "registry.hpp"

// systems
#include "./systems/physics_system.hpp"
#include "./systems/world_system.hpp"
#include "render_system.hpp"

class BoardScene : public Scene {
 public:
  BoardScene();

  // releases all associated resources
  ~BoardScene();

  // starts the scene
  void init(std::shared_ptr<WindowManager> window_manager,
            std::function<void(BoardRegistry)> on_scene_end);

  // steps the scene ahead by delta (in milliseconds)
  bool step(float delta);

  // resets the board
  void reset_scene();

  // input callback for mouse and key presses
  void on_key(int key, int action, int mod);

  // input callback for mouse movement
  void on_mouse_move(vec2 pos);

 private:
  // holds the scene state
  bool displayed_story = false;
  bool waiting_for_continue = true;
  int count = 0;
  bool help_on = true;
  std::string story = {
      "The Doge kingdom is in a war with the Cat kingdom. The cats are too "
      "strong and now Doge is losing most of the fights. The King of Doge "
      "decided to start a plan called \"Party time\" to win this war. You and "
      "other players are on a spaceship and the secret mission is to sneak "
      "into their planet to do a secret attack and make their kingdom into a "
      "party to win the war! The king said the person who earns the most "
      "points will have unlimited snacks in their lifetime. So compete with "
      "other players to earn this reward!"};

  std::vector<std::string> help = {
      "Space: roll the dice",    "F1: to add players",
      "Choose the direction by", "    Arrow keys at intersection",
      "red decrease 3 points",   "blue increase 3 points",
      "S to save game",          "L to load game"};

  std::shared_ptr<BoardRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // systems the scene uses
  std::shared_ptr<BoardWorldSystem> world;
  std::shared_ptr<BoardPhysicsSystem> physics;
  std::shared_ptr<RenderSystem> renderer;

  // callback called when the scene ends
  std::function<void(BoardRegistry)> on_scene_end_callback_ptr;

  // ends the scene
  void end();
};
