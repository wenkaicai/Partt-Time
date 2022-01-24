#include "window_manager.hpp"

WindowManager::WindowManager() {}

WindowManager::~WindowManager() {
  // Destroy music components
  // if (background_music != nullptr)
  //	Mix_FreeMusic(background_music);
  Mix_CloseAudio();

  // Close the window
  glfwDestroyWindow(window);
}

// Debugging
namespace {
void glfw_err_cb(int error, const char *desc) {
  fprintf(stderr, "%d: %s", error, desc);
}
}  // namespace

GLFWwindow *WindowManager::create_window(int width, int height) {
  ///////////////////////////////////////
  // Initialize GLFW
  glfwSetErrorCallback(glfw_err_cb);
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW");
    return nullptr;
  }

  //-------------------------------------------------------------------------
  // If you are on Linux or Windows, you can change these 2 numbers to 4 and 3
  // and enable the glDebugMessageCallback to have OpenGL catch your mistakes
  // for you. GLFW / OGL Initialization
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, 0);

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  // get closest 16:9 aspect ratio
  // https://stackoverflow.com/questions/53954028/how-do-i-work-out-the-nearest-resolution-with-169-aspect-ratio-python-3
  float tempWidth = mode->width;
  float tempHeight = mode->height;
  if (tempWidth / tempHeight < 16.0f / 9.0f) {
    width = 16.0f * (tempHeight / 16.0f);
    height = 9.0f * (tempHeight / 16.0f);
  } else {
    width = 16.0f * (tempHeight / 9.0f);
    height = 9.0f * (tempHeight / 9.0f);
  }

  // Create the main window (for rendering, keyboard, and mouse input)
  float scale = 1.5;
  window =
      glfwCreateWindow(width / scale, height / scale, "Party Time", nullptr, nullptr);
  if (window == nullptr) {
    fprintf(stderr, "Failed to glfwCreateWindow");
    return nullptr;
  }

  // Setting callbacks to member functions (that's why the redirect is needed)
  // Input is handled using GLFW, for more info see
  // http://www.glfw.org/docs/latest/input_guide.html
  glfwSetWindowUserPointer(window, this);
  auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
    ((WindowManager *)glfwGetWindowUserPointer(wnd))->on_key(_0, _2, _3);
  };
  auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
    ((WindowManager *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1});
  };
  glfwSetKeyCallback(window, key_redirect);
  glfwSetCursorPosCallback(window, cursor_pos_redirect);

  //////////////////////////////////////
  // Loading music and sounds with SDL
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "Failed to initialize SDL Audio");
    return nullptr;
  }
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
    fprintf(stderr, "Failed to open audio device");
    return nullptr;
  }

  // handle closing the window from OS
  glfwSetWindowCloseCallback(window, glfwDestroyWindow);

  return window;
}

void WindowManager::on_key(int key, int action, int mod) {
  // FIXME: check if the callback is a null pointer
  on_key_callback_ptr(key, action, mod);
}

void WindowManager::on_mouse_move(vec2 pos) {
  // FIXME: check if the callback is a null pointer
  on_mouse_move_callback_ptr(pos);
}

void WindowManager::set_on_key_callback(std::function<void(int, int, int)> cb) {
  on_key_callback_ptr = cb;
}

void WindowManager::set_on_mouse_move_callback(
    std::function<void(glm::vec2)> cb) {
  on_mouse_move_callback_ptr = cb;
}

GLFWwindow *WindowManager::get_window() { return window; }
