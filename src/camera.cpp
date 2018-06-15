#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 targetPosition, int width,
               int height)
    : pos(position), width(width), height(height), speed(5.0f) {
  proj = glm::perspective(
      glm::radians(fov), static_cast<float>(width) / static_cast<float>(height),
      zNear, zFar);
  glm::vec3 direction = glm::normalize(targetPosition - position);
  pitch = asinf(direction.y);
  yaw = atan2(direction.x, direction.z);
  updateDirection(0.0f);
  updateView(0.0f);
}

void Camera::updateView(float deltaTime) {
  view = glm::lookAt(pos, dir + pos, up);
}

void Camera::updateDirection(float deltaTime) {
  dir = glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
  dir = glm::normalize(dir);
  right = glm::vec3(sin(yaw - M_PI / 2.0f), 0.0f, cos(yaw - M_PI / 2.0f));
  right = glm::normalize(right);
  up = glm::normalize(glm::cross(right, dir));
}

void Camera::updateMouse(Env &env, float deltaTime) {
  if (env.inputHandler.mstate == MouseState::Virtual) {
    if (env.inputHandler.mousex != mouse_pos.x ||
        env.inputHandler.mousey != mouse_pos.y) {
      old_mouse_pos = mouse_pos;
      mouse_pos = glm::vec2(env.inputHandler.mousex, env.inputHandler.mousey);
      if (env.ignore_mouse) {
        old_mouse_pos = mouse_pos;
        env.ignore_mouse = false;
      } else {
        yaw += 0.1f * deltaTime * (old_mouse_pos.x - mouse_pos.x);
        pitch += 0.1f * deltaTime * (old_mouse_pos.y - mouse_pos.y);
        pitch = glm::clamp(pitch, -1.5f, 1.5f);
      }
    }
  }
}

void Camera::updateKeyboard(Env &env, float deltaTime) {
  if (env.inputHandler.kstate == KeybrState::Enabled) {
    if (env.inputHandler.keys[GLFW_KEY_LEFT_SHIFT]) {
      speed = 20.0f;
    } else {
      speed = 5.0f;
    }
    if (env.inputHandler.keys[GLFW_KEY_UP] ||
        env.inputHandler.keys[GLFW_KEY_W]) {
      glm::vec3 tmp = dir * speed * deltaTime;
      pos = pos + tmp;
    }
    if (env.inputHandler.keys[GLFW_KEY_DOWN] ||
        env.inputHandler.keys[GLFW_KEY_S]) {
      glm::vec3 tmp = dir * speed * deltaTime;
      pos = pos - tmp;
    }
    if (env.inputHandler.keys[GLFW_KEY_RIGHT] ||
        env.inputHandler.keys[GLFW_KEY_D]) {
      glm::vec3 tmp = right * speed * deltaTime;
      pos = pos + tmp;
    }
    if (env.inputHandler.keys[GLFW_KEY_LEFT] ||
        env.inputHandler.keys[GLFW_KEY_A]) {
      glm::vec3 tmp = right * speed * deltaTime;
      pos = pos - tmp;
    }
  }
}

void Camera::update(Env &env, float deltaTime) {
  if (width != env.width || height != env.height) {
    proj = glm::perspective(
        glm::radians(fov),
        static_cast<float>(env.width) / static_cast<float>(env.height), zNear,
        zFar);
    width = env.width;
    height = env.height;
  }
  updateMouse(env, deltaTime);
  updateDirection(deltaTime);
  updateKeyboard(env, deltaTime);
  updateView(deltaTime);
}

float Camera::getAspectRatio() {
  return (static_cast<float>(width) / static_cast<float>(height));
}
