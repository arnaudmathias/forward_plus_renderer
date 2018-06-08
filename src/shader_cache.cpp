#include "shader_cache.hpp"

ShaderCache::ShaderCache(void) {
  _shaders.emplace("default", new Shader("shaders/default"));
  _shaders.emplace("depthprepass", new Shader("shaders/depthprepass"));
  _shaders.emplace("text", new Shader("shaders/text"));
}

ShaderCache::ShaderCache(ShaderCache const& src) { *this = src; }

ShaderCache::~ShaderCache(void) {
  for (auto it = _shaders.begin(); it != _shaders.end(); it++) {
    delete it->second;
  }
}

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

Shader* ShaderCache::getShader(const std::string& shader_key) {
  auto shader_it = _shaders.find(shader_key);
  if (shader_it != _shaders.end()) {
    return (shader_it->second);
  }
  return (nullptr);
}
