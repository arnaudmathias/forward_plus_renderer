#pragma once
#define TILE_SIZE 16
#define NUM_LIGHTS 8
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <iomanip>
#include <iostream>

struct Material {
  glm::vec4 ambient = {};
  glm::vec4 diffuse = {};
  glm::vec4 specular = {};
  glm::vec4 transmittance = {};
  glm::vec4 emission = {};
  float specular_power = 1.0f;
  float index_of_refraction = 1.0f;
  float opacity = 1.0f;

  float roughness = 0.0f;
  float metallic = 0.0f;
  float sheen = 0.0f;
  float clearcoat_thickness = 0.0f;
  float clearcoat_roughness = 0.0f;
  float anisotropy = 0.0f;
  float anisotropy_rotation = 0.0f;

  glm::vec2 padding = {};
};

struct Light {
  glm::vec3 position = {};
  float radius = 1.0f;
  glm::vec3 color = {};
  float intensity = 1.0f;
};

struct Lights {
  struct Light lights[NUM_LIGHTS] = {};
};

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  glm::vec3 normal = {0, 0, 0};
  glm::vec2 uv = {0, 0};
  glm::vec3 tangent = {0, 0, 0};
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
