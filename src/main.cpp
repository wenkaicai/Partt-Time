
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <memory>
#include <thread>

// internal
#include "scene_manager.hpp"
#include "window_manager.hpp"

using Clock = std::chrono::high_resolution_clock;

// get current vertical resolution,ensure
const int window_height_px = 1200;
const int window_width_px = 675;

// Entry point
int main() {
  // Global systems

  std::shared_ptr<WindowManager> window_manager =
      std::make_shared<WindowManager>();

  // Initializing window
  auto window =
      window_manager->create_window(window_width_px, window_height_px);

  if (!window) {
    // Time to read the error message
    printf("Press any key to exit");
    getchar();
    return EXIT_FAILURE;
  }

  // initialize board scene
  SceneManager scene_manager;
  scene_manager.init(window_manager);

  // variable timestep loop
  auto t = Clock::now();
  long frame_counter = 0;
  auto frame_timer = Clock::now();

  while (!scene_manager.is_quit_game()) {
    // Processes system messages, if this wasn't present the window would become
    // unresponsive
    glfwPollEvents();

    // Calculating elapsed times in milliseconds from the previous iteration
    auto now = Clock::now();
    float elapsed_ms =
        (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t))
            .count() /
        1000;
    t = now;

    frame_counter += 1;
    float last_fps_update =
        (float)(std::chrono::duration_cast<std::chrono::seconds>(now -
                                                                 frame_timer))
            .count();
    if (last_fps_update > 5) {
      printf("fps: %d\n", int(frame_counter / last_fps_update));
      frame_timer = now;
      frame_counter = 0;
    }

    scene_manager.step_current_scene(elapsed_ms);

    if (scene_manager.rounds_left == 0) break;
  }

  printf("Thank you for playing our game!\n");

  return EXIT_SUCCESS;
}
