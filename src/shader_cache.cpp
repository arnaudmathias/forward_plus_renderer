#include "shader_cache.hpp"

ShaderCache::ShaderCache(void) {
  _shaders.emplace("anim", new Shader("shaders/anim"));
  _shaders.emplace("anim_debug", new Shader("shaders/anim_debug"));
  _shaders.emplace("billboard", new Shader("shaders/billboard"));
  _shaders.emplace("skybox", new Shader("shaders/skybox"));
  _shaders.emplace("ui", new Shader("shaders/ui"));
  _shaders.emplace("text", new Shader("shaders/text"));
  _shaders.emplace("void_model", new Shader("shaders/void_model"));
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
