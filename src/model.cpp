#include "model.hpp"

Model::Model(enum ModelType type) : _type(type) {
  attrib.shader_key = "void_model";
}

Model::Model(Model const& rhs) { *this = rhs; }

Model& Model::operator=(Model const& rhs) {
  if (this != &rhs) {
  }
  return (*this);
}

Model::~Model(void) {
  for (auto& vao : attrib.vaos) {
    delete vao;
  }
}
