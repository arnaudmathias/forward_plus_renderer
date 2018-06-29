#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "shader.hpp"

class ShaderCache {
 public:
  ShaderCache(void);
  ShaderCache(ShaderCache const& src);
  ~ShaderCache(void);
  ShaderCache& operator=(ShaderCache const& rhs);

  std::shared_ptr<Shader> getShader(const std::string& shader);
  int getShaderID(const std::string& shader_key);
  void update();

 private:
  std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
};
