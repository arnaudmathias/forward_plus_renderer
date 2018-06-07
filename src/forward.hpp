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

struct Material {
  glm::vec4 global_ambient;
  glm::vec4 ambient_color;
  glm::vec4 emissive_color;
  glm::vec4 diffuse_color;
  glm::vec4 specular_color;
  glm::vec4 reflectance;
  float opacity;
  float specular_power;
  float index_of_refraction;
  bool has_ambient_texture;
  bool has_emissive_texture;
  bool has_diffuse_texture;
  bool has_specular_texture;
  bool has_specular_power_texture;
  bool has_normal_texture;
  bool has_bump_texture;
  bool has_opacity_texture;
  float bump_intensity;
  float specular_Scale;
  float alpha_threshold;
  glm::vec2 padding;
};

struct Light {
  glm::vec4 position_ws;
  glm::vec4 direction_ws;
  glm::vec4 position_vs;
  glm::vec4 direction_vs;
  glm::vec4 color;
  float spotlight_angle;
  float range;
  float intensity;
  unsigned int enabled;
  unsigned int selected;
  unsigned int type;
  glm::vec2 padding;
};

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  glm::vec3 normal = {0, 0, 0};
  glm::vec2 uv = {0, 0};

  Vertex() : position({0.0f, 0.0f, 0.0f}){};
  Vertex(Vertex const& src) { *this = src; }

  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      this->position = rhs.position;
      this->normal = rhs.normal;
      this->uv = rhs.uv;
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
