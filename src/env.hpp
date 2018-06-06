#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
// --
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include <sstream>

class InputHandler {
 public:
  std::array<bool, 1024> keys = {{0}};
  bool mouseDisabled = false;
  bool keybrDisabled = false;
  float mousex;
  float mousey;
  std::array<bool, 12> mouse_keys = {{0}};
};

class Env {
 public:
  GLFWwindow *window;
  InputHandler inputHandler;
  Env();
  Env(unsigned short width, unsigned short height);
  ~Env();

  void toggleFullscreen();
  void update();  // Called once per frame
  float getDeltaTime() const;
  float getAbsoluteTime() const;
  float getFrame() const;
  float getFPS() const;
  int width;
  int height;
  bool has_resized;

 private:
  void updateFpsCounter();
  void setupContext();
  void setupWindow();
  void setupWindowHint();
  float _absoluteTime;
  float _deltaTime;
  float _frame;
  float _fps;
  bool _fullscreen = false;
  int _window_width;
  int _window_height;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseKeyCallback(GLFWwindow *window, int button, int action, int mods);

static void APIENTRY openglCallbackFunction(GLenum source, GLenum type,
                                            GLuint id, GLenum severity,
                                            GLsizei length,
                                            const GLchar *message,
                                            const void *userParam);
