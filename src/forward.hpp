#pragma once
#define TILE_SIZE 16
#define NUM_LIGHTS 256
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
  glm::vec4 global_ambient = {};
  glm::vec4 ambient_color = {};
  glm::vec4 emissive_color = {};
  glm::vec4 diffuse_color = {};
  glm::vec4 specular_color = {};
  glm::vec4 reflectance = {};
  float opacity = 1.0f;
  float specular_power = 1.0f;
  float index_of_refraction = 1.0f;
  unsigned int has_ambient_texture = 0;
  unsigned int has_emissive_texture = 0;
  unsigned int has_diffuse_texture = 0;
  unsigned int has_specular_texture = 0;
  unsigned int has_specular_power_texture = 0;
  unsigned int has_normal_texture = 0;
  unsigned int has_bump_texture = 0;
  unsigned int has_opacity_texture = 0;
  float bump_intensity = 1.0f;
  float specular_scale = 1.0f;
  float alpha_threshold = 0.8f;
  glm::vec2 padding = {};
  Material() = default;
  Material(Material const& src) { *this = src; }
  Material& operator=(Material const& rhs) {
    if (this != &rhs) {
      global_ambient = rhs.global_ambient;
      ambient_color = rhs.ambient_color;
      emissive_color = rhs.emissive_color;
      diffuse_color = rhs.diffuse_color;
      specular_color = rhs.specular_color;
      reflectance = rhs.specular_color;
      opacity = rhs.opacity;
      specular_power = rhs.specular_power;
      index_of_refraction = rhs.index_of_refraction;
      has_ambient_texture = rhs.has_ambient_texture;
      has_emissive_texture = rhs.has_emissive_texture;
      has_diffuse_texture = rhs.has_diffuse_texture;
      has_specular_texture = rhs.has_specular_texture;
      has_specular_power_texture = rhs.has_specular_power_texture;
      has_normal_texture = rhs.has_normal_texture;
      has_bump_texture = rhs.has_bump_texture;
      has_opacity_texture = rhs.has_opacity_texture;
      bump_intensity = rhs.bump_intensity;
      specular_scale = rhs.specular_scale;
      alpha_threshold = rhs.alpha_threshold;
    }
    return (*this);
  }
};

struct Light {
  glm::vec3 position = {};
  float radius = 1.0f;
  glm::vec3 color = {};
  float intensity = 1.0f;

  Light() = default;
  Light(Light const& src) { *this = src; }

  Light& operator=(Light const& rhs) {
    if (this != &rhs) {
      position = rhs.position;
      radius = rhs.radius;
      color = rhs.color;
      intensity = rhs.intensity;
    }
    return (*this);
  };
};

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  glm::vec3 normal = {0, 0, 0};
  glm::vec2 uv = {0, 0};
  glm::vec3 tangent = {0, 0, 0};

  Vertex() = default;
  Vertex(Vertex const& src) { *this = src; }

  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      position = rhs.position;
      normal = rhs.normal;
      uv = rhs.uv;
      tangent = rhs.tangent;
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
