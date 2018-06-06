#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 targetPosition, int width,
               int height)
    : pos(position),
      width(width),
      height(height),
      mouseInit(false),
      zNear(0.1f),
      zFar(1000.0f),
      mouseMoved(false),
      speed(1.0f) {
  proj = glm::perspective(
      glm::radians(80.0f),
      static_cast<float>(width) / static_cast<float>(height), zNear, zFar);
  glm::vec3 direction = glm::normalize(targetPosition - position);
  verAngle = asinf(direction.y);
  horAngle = atan2(direction.x, direction.z);
  updateMatrix(0.0f);
}

void Camera::updateMatrix(float deltaTime) {
  if (mouseMoved) {
    horAngle += 0.1f * deltaTime * (oldMouseXpos - mouseXpos);
    verAngle += 0.1f * deltaTime * (oldMouseYpos - mouseYpos);
    verAngle = glm::clamp(verAngle, -1.5f, 1.5f);
    mouseMoved = false;
  }
  dir = glm::vec3(cos(verAngle) * sin(horAngle), sin(verAngle),
                  cos(verAngle) * cos(horAngle));
  dir = glm::normalize(dir);
  right =
      glm::vec3(sin(horAngle - 3.14f / 2.0f), 0, cos(horAngle - 3.14f / 2.0f));
  right = glm::normalize(right);
  up = glm::normalize(glm::cross(right, dir));
  view = glm::lookAt(pos, dir + pos, up);
}

void Camera::rotate(float hor, float ver) {
  horAngle += hor;
  verAngle += ver;
}

void Camera::update(Env &env, float deltaTime) {
  if (width != env.width || height != env.height) {
    proj = glm::perspective(
        glm::radians(80.0f),
        static_cast<float>(env.width) / static_cast<float>(env.height), zNear,
        zFar);
    width = env.width;
    height = env.height;
  }
  if (env.inputHandler.keybrDisabled == false) {
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
      glm::vec3 right = glm::cross(up, dir);
      glm::vec3 tmp = right * speed * deltaTime;
      pos = pos - tmp;
    }
    if (env.inputHandler.keys[GLFW_KEY_LEFT] ||
        env.inputHandler.keys[GLFW_KEY_A]) {
      glm::vec3 right = glm::cross(up, dir);
      glm::vec3 tmp = right * speed * deltaTime;
      pos = pos + tmp;
    }
  }
  if (env.inputHandler.mouseDisabled == false) {
    if (env.inputHandler.mousex != mouseXpos ||
        env.inputHandler.mousey != mouseYpos) {
      if (mouseInit) {
        oldMouseXpos = mouseXpos;
        oldMouseYpos = mouseYpos;
      } else {
        oldMouseXpos = env.inputHandler.mousex;
        oldMouseYpos = env.inputHandler.mousey;
        mouseInit = true;
      }
      mouseXpos = env.inputHandler.mousex;
      mouseYpos = env.inputHandler.mousey;
      if (env.has_resized) {
        env.has_resized = false;
      } else {
        this->mouseMoved = true;
      }
    }
  }
  updateMatrix(deltaTime);
}

float Camera::getAspectRatio() {
  return (static_cast<float>(width) / static_cast<float>(height));
}
