#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <iomanip>
#include <iostream>

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  glm::vec3 normal = {0, 0, 0};
  glm::ivec4 bone_ids = {0, 0, 0, 0};
  glm::vec4 weights = {0, 0, 0, 0};

  Vertex() : position({0.0f, 0.0f, 0.0f}){};
  Vertex(Vertex const& src) { *this = src; }

  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      this->position = rhs.position;
      this->normal = rhs.normal;
      this->bone_ids = rhs.bone_ids;
      this->weights = rhs.weights;
    }
    return (*this);
  };
};

static std::ostream& operator<<(std::ostream& o, glm::vec3 const& v) {
  o << glm::to_string(v);
  return (o);
}

static std::ostream& operator<<(std::ostream& o, glm::vec4 const& v) {
  o << glm::to_string(v);
  return (o);
}

static std::ostream& operator<<(std::ostream& o, glm::quat const& v) {
  o << glm::to_string(v);
  return (o);
}

static std::ostream& operator<<(std::ostream& o, glm::mat4 const& v) {
  o << glm::to_string(v);
  return (o);
}
