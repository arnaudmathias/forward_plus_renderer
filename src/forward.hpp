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
  unsigned int has_ambient_texture;
  unsigned int has_emissive_texture;
  unsigned int has_diffuse_texture;
  unsigned int has_specular_texture;
  unsigned int has_specular_power_texture;
  unsigned int has_normal_texture;
  unsigned int has_bump_texture;
  unsigned int has_opacity_texture;
  float bump_intensity;
  float specular_scale;
  float alpha_threshold;
  glm::vec2 padding;
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

  Light() = default;
  Light(Light const& src) { *this = src; }

  Light& operator=(Light const& rhs) {
    if (this != &rhs) {
      position_ws = rhs.position_ws;
      direction_ws = rhs.direction_ws;
      position_vs = rhs.position_vs;
      direction_vs = rhs.direction_vs;
      color = rhs.color;
      spotlight_angle = rhs.spotlight_angle;
      range = rhs.range;
      intensity = rhs.intensity;
      enabled = rhs.enabled;
      selected = rhs.selected;
      type = rhs.type;
    }
    return (*this);
  };
};

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  glm::vec3 normal = {0, 0, 0};
  glm::vec2 uv = {0, 0};

  Vertex() = default;
  Vertex(Vertex const& src) { *this = src; }

  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      position = rhs.position;
      normal = rhs.normal;
      uv = rhs.uv;
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
