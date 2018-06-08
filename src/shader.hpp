#pragma once
#include <sys/stat.h>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include "env.hpp"

const int shader_types[4] = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER,
                             GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER};

const std::string shader_extensions[4] = {".vert", ".geom", ".frag", ".comp"};

struct ShaderFile {
  std::string filename;
  std::time_t last_modification;
};

class Shader {
 public:
  Shader(std::string shader);
  Shader(Shader const &src);
  ~Shader(void);
  Shader &operator=(Shader const &rhs);

  GLuint id;
  void use() const;
  void reload();

 private:
  Shader(void);
  GLuint loadShader(std::string &shader);
  GLuint compileShader(const std::string source, std::string filename,
                       GLuint shaderType);
  GLuint linkShaders(const std::array<GLuint, 4> shader_ids);
  const std::string getShaderSource(std::string filename);
  ShaderFile _shaders[4];
};

void printShaderError(GLuint shade, std::string filename);
void printLinkError(GLuint program);
bool file_exists(std::string filename);
std::time_t getLastModificationTime(std::string filename);
