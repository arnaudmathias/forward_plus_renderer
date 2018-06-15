#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
// --
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include <sstream>

enum class KeybrState { Enabled, Disabled };

enum class MouseState { Normal, Hidden, Virtual };

struct InputHandler {
  KeybrState kstate = KeybrState::Enabled;
  MouseState mstate = MouseState::Virtual;

  float mousex = 0.0f;
  float mousey = 0.0f;

  std::array<bool, 1024> keys = {{0}};
  std::array<bool, 12> mouse_keys = {{0}};
  bool mouse_in_window = true;

  bool window_focused = true;
  int window_width = 0;
  int window_height = 0;
  int window_pos_x = 0;
  int window_pos_y = 0;
};

class Env {
 public:
  GLFWwindow *window = nullptr;
  InputHandler inputHandler;
  Env() noexcept;
  Env(unsigned short width, unsigned short height);
  ~Env();

  void toggleFullscreen();
  void changeMouseState(MouseState state);
  void update();  // Called once per frame

  float getDeltaTime() const;
  float getAbsoluteTime() const;
  unsigned int getFrame() const;
  float getFPS() const;

  int width = 0;
  int height = 0;

  // Handle sudden change in mouse virtual position during screen size
  // transition or mouse state change
  // The caller must ignore the new mouse position and reset the variable
  bool ignore_mouse = true;

 private:
  void updateFpsCounter();
  void setupContext();
  void setupWindow();
  void setupWindowHint();
  float _absoluteTime = 0.0f;
  float _deltaTime = 0.0f;
  unsigned int _frame = 0;
  float _fps = 0.0f;
  bool _fullscreen = false;
  int _windowed_width = 1280;
  int _windowed_height = 720;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseKeyCallback(GLFWwindow *window, int button, int action, int mods);
void mouseEnterCallback(GLFWwindow *window, int has_entered);

void windowFocusCallback(GLFWwindow *window, int focused);
void windowResizeCallback(GLFWwindow *window, int width, int height);
void windowPositionCallback(GLFWwindow *window, int x, int y);

static void APIENTRY openglCallbackFunction(GLenum source, GLenum type,
                                            GLuint id, GLenum severity,
                                            GLsizei length,
                                            const GLchar *message,
                                            const void *userParam);

static void GL_DUMP_ERROR(std::string message) {
  int glErrorCode = 0;
  if ((glErrorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (glErrorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        error = "STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        error = "STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << message << " (" << glErrorCode << ": " << error << ")\n";
  }
}
