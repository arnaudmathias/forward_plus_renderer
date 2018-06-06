#pragma once
#include <cstring>
#include <unordered_map>
#include <vector>
#include "anim.hpp"
#include "renderer.hpp"

struct Model {
  Model(enum ModelType type);
  Model(Model const& src);
  Model& operator=(Model const& rhs);
  ~Model();

  render::Attrib attrib;

 private:
  Model(void) = default;
  enum ModelType _type;
  render::Attrib _animAttrib;
};
