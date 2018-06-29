#include "shader_cache.hpp"

ShaderCache::ShaderCache(void) {
  _shaders.emplace("default", std::make_shared<Shader>("shaders/default"));
  _shaders.emplace("shading", std::make_shared<Shader>("shaders/shading"));
  _shaders.emplace("depthprepass",
                   std::make_shared<Shader>("shaders/depthprepass"));
  _shaders.emplace("octahedron",
                   std::make_shared<Shader>("shaders/octahedron"));
  _shaders.emplace("text", std::make_shared<Shader>("shaders/text"));
  if (GLAD_GL_VERSION_4_3) {
    // Compute shaders
    _shaders.emplace("lightculling",
                     std::make_shared<Shader>("shaders/lightculling"));
  }
}

ShaderCache::ShaderCache(ShaderCache const& src) { *this = src; }

ShaderCache::~ShaderCache(void) {}

ShaderCache& ShaderCache::operator=(ShaderCache const& rhs) {
  if (this != &rhs) {
  }
  return (*this);
}

void ShaderCache::update() {
  for (auto it = _shaders.begin(); it != _shaders.end(); it++) {
    it->second->reload();
  }
}

std::shared_ptr<Shader> ShaderCache::getShader(const std::string& shader_key) {
  auto shader_it = _shaders.find(shader_key);
  if (shader_it != _shaders.end()) {
    return (shader_it->second);
  }
  return (nullptr);
}

int ShaderCache::getShaderID(const std::string& shader_key) {
  auto shader_it = _shaders.find(shader_key);
  if (shader_it != _shaders.end()) {
    return (shader_it->second->id);
  }
  return (-1);
}
