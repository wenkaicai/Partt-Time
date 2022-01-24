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
#include "../render_system.hpp"
#include "./systems/physics_system.hpp"
#include "./systems/world_system.hpp"

class PlanitScene : public Scene {
 public:
  PlanitScene();

  // releases all associated resources
  ~PlanitScene();

  // starts the scene
  void init(std::shared_ptr<WindowManager> window_manager,
            std::function<void(PlanitRegistry)> on_scene_end);

  // steps the scene ahead by delta (in milliseconds)
  bool step(float delta);

  // rests the scene
  void reset_scene();

  // input callback for mouse and key presses
  void on_key(int key, int action, int mod);

  // input callback for mouse movement
  void on_mouse_move(vec2 pos);

 private:
  // holds the scene state
  std::shared_ptr<PlanitRegistry> registry;

  std::shared_ptr<WindowManager> window_manager;

  // systems the scene uses
  std::shared_ptr<PlanitWorldSystem> world;
  std::shared_ptr<PlanitPhysicsSystem> physics;
  std::shared_ptr<RenderSystem> renderer;

  // callback called when the scene ends
  std::function<void(PlanitRegistry)> on_scene_end_callback_ptr;

  // ends the scene
  void end();
};
