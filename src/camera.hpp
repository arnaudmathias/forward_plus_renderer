#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <array>
#include "env.hpp"
#include "forward.hpp"

class Camera {
 public:
  glm::vec3 pos = {};
  glm::vec3 dir = {};
  glm::vec3 right = {};
  glm::vec3 up = {};
  glm::mat4 proj = {};
  glm::mat4 view = {};
  int width = 0;
  int height = 0;
  glm::vec2 old_mouse_pos = {};
  glm::vec2 mouse_pos = {};
  float zNear = 0.1f;
  float zFar = 50.0f;
  float fov = 80.0f;

  Camera(glm::vec3 pos, glm::vec3 target, int width = 1024, int height = 1024);
  void update(Env &env, float deltaTime);

  void updateView(float deltaTime);
  void updateDirection(float deltaTime);

  void updateMouse(Env &env, float deltaTime);
  void updateKeyboard(Env &env, float deltaTime);

  float getAspectRatio();

 private:
  void updateMatrix(float deltaTime);
  float pitch;
  float yaw;
  float speed;
  float lastVelocity = 0.0f;
};
