#include "env.hpp"

Env::Env() noexcept : Env(0, 0) {}

Env::Env(unsigned int w, unsigned int h) : width(w), height(h) {
  if (!glfwInit()) return;
  setupWindowHint();
  std::string window_name = "Forward+ renderer";
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(primary_monitor);
  if (width == 0 && height == 0) {
    // Fullscreen
    width = static_cast<unsigned int>(mode->width);
    height = static_cast<unsigned int>(mode->height);
    _fullscreen = true;
  } else {
    // Windowed
    _windowed_width = width;
    _windowed_height = height;
    primary_monitor = NULL;
  }
  inputHandler.window_pos_x = (mode->width / 2) - (_windowed_width / 2);
  inputHandler.window_pos_y = (mode->height / 2) - (_windowed_height / 2);
  window = glfwCreateWindow(width, height, window_name.c_str(), primary_monitor,
                            NULL);
  if (window) {
    glfwSetWindowMonitor(window, primary_monitor, inputHandler.window_pos_x,
                         inputHandler.window_pos_y, width, height, 0);
  }
  inputHandler.mousex = static_cast<float>(width / 2);
  inputHandler.mousey = static_cast<float>(height / 2);
  inputHandler.window_width = width;
  inputHandler.window_height = height;
  if (!window) {
    std::cerr << "Could not create window\n";
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
  }
  std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  setupWindow();
  setupContext();
}

Env::~Env() { glfwTerminate(); }

void Env::toggleFullscreen() {
  _fullscreen = !_fullscreen;
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(primary_monitor);
  if (_fullscreen) {
    glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width,
                         mode->height, mode->refreshRate);
    width = mode->width;
    height = mode->height;
  } else {
#if defined(_WIN32)
    inputHandler.window_pos_x = (mode->width / 2) - (_windowed_width / 2);
    inputHandler.window_pos_y = (mode->height / 2) - (_windowed_height / 2);
#endif
    glfwSetWindowMonitor(window, NULL, inputHandler.window_pos_x,
                         inputHandler.window_pos_y, _windowed_width,
                         _windowed_height, mode->refreshRate);

    width = _windowed_width;
    height = _windowed_height;
  }
  ignore_mouse = true;
  // Query and update framebuffer size
  int wframe, hframe;
  glfwGetFramebufferSize(window, &wframe, &hframe);
  glViewport(0, 0, wframe, hframe);
}

void Env::changeMouseState(MouseState state) {
  switch (state) {
    case MouseState::Normal:
      ignore_mouse = true;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPos(window, static_cast<double>(width / 2),
                       static_cast<double>(height / 2));
      break;
    case MouseState::Hidden:
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      break;
    case MouseState::Virtual:
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      break;
    default:
      break;
  }
  inputHandler.mstate = state;
}

void Env::setupWindowHint() {
#if defined(__APPLE__)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#elif defined(_WIN32) || defined(__linux__)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
}

void Env::setupWindow() {
  if (window != nullptr) {
    glfwSetWindowUserPointer(window, &inputHandler);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseKeyCallback);
    glfwSetCursorEnterCallback(window, mouseEnterCallback);

    glfwSetWindowFocusCallback(window, windowFocusCallback);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetWindowPosCallback(window, windowPositionCallback);

    changeMouseState(MouseState::Virtual);
  }
}

void Env::setupContext() {
  glfwSwapInterval(0);
  /*glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(openglCallbackFunction, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        true);
  while (glGetError() != GL_NO_ERROR)
    ;  // Flush gl_error
  */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
}

void Env::update() {
  updateFpsCounter();
  static double previousTime = glfwGetTime();
  static int frame_count;
  double currentTime = glfwGetTime();
  double deltaTime = currentTime - previousTime;
  previousTime = currentTime;

  _deltaTime = static_cast<float>(deltaTime);
  _absoluteTime = static_cast<float>(currentTime);
  _frame++;

  if (inputHandler.window_focused == false &&
      inputHandler.mstate == MouseState::Virtual) {
    changeMouseState(MouseState::Normal);
  }
  if (inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT] &&
      inputHandler.mstate == MouseState::Normal) {
    if (inputHandler.mouse_in_window) {
      changeMouseState(MouseState::Virtual);
    }
    inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT] = false;
  }
  if (inputHandler.keys[GLFW_KEY_ESCAPE]) {
    inputHandler.keys[GLFW_KEY_ESCAPE] = false;
    if (inputHandler.mstate == MouseState::Virtual) {
      changeMouseState(MouseState::Normal);
    } else {
      glfwSetWindowShouldClose(window, 1);
    }
  }
  if (inputHandler.keys[GLFW_KEY_F]) {
    inputHandler.keys[GLFW_KEY_F] = false;
    toggleFullscreen();
  }
  if (inputHandler.window_width != width ||
      inputHandler.window_height != height) {
    width = inputHandler.window_width;
    height = inputHandler.window_height;
    // Query and update framebuffer size
    int wframe, hframe;
    glfwGetFramebufferSize(window, &wframe, &hframe);
    glViewport(0, 0, wframe, hframe);
  }
}

void Env::updateFpsCounter() {
  static double previous_seconds = glfwGetTime();
  static int frame_count;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 1.0) {
    previous_seconds = current_seconds;
    this->_fps =
        static_cast<float>(frame_count) / static_cast<float>(elapsed_seconds);
    std::string tmp = std::to_string(_fps);
    glfwSetWindowTitle(window, tmp.c_str());
    frame_count = 0;
  }
  frame_count++;
}

float Env::getDeltaTime() const { return (this->_deltaTime); }

float Env::getAbsoluteTime() const { return (this->_absoluteTime); }

unsigned int Env::getFrame() const { return (this->_frame); }

float Env::getFPS() const { return (this->_fps); }

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode) {
  (void)scancode;
  (void)mode;
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    inputHandler->keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    inputHandler->keys[key] = false;
  }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->mousex = static_cast<float>(xpos);
  inputHandler->mousey = static_cast<float>(ypos);
}

void mouseKeyCallback(GLFWwindow *window, int button, int action, int mods) {
  (void)mods;
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    inputHandler->mouse_keys[button] = true;
  } else if (action == GLFW_RELEASE) {
    inputHandler->mouse_keys[button] = false;
  }
}

void mouseEnterCallback(GLFWwindow *window, int has_entered) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->mouse_in_window = has_entered == GLFW_TRUE ? true : false;
}

void windowFocusCallback(GLFWwindow *window, int focused) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->window_focused = focused == GLFW_TRUE ? true : false;
}

void windowResizeCallback(GLFWwindow *window, int width, int height) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->window_width = width;
  inputHandler->window_height = height;
}

void windowPositionCallback(GLFWwindow *window, int x, int y) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->window_pos_x = x;
  inputHandler->window_pos_y = y;
}

static void APIENTRY openglCallbackFunction(GLenum source, GLenum type,
                                            GLuint id, GLenum severity,
                                            GLsizei length,
                                            const GLchar *message,
                                            const void *userParam) {
  (void)source;
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)userParam;
  fprintf(stderr, "%s\n", message);
  if (severity == GL_DEBUG_SEVERITY_HIGH) {
    fprintf(stderr, "Aborting...\n");
    abort();
  }
}
