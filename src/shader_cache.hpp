#pragma once
#include <string>
#include <unordered_map>
#include "shader.hpp"

class ShaderCache {
 public:
  ShaderCache(void);
  ShaderCache(ShaderCache const& src);
  ~ShaderCache(void);
  ShaderCache& operator=(ShaderCache const& rhs);

  Shader* getShader(const std::string& shader);
  void update();

 private:
  std::unordered_map<std::string, Shader*> _shaders;
};
