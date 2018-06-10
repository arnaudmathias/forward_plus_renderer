#pragma once
#include <assert.h>
#include <cstring>
#include <map>
#include <queue>
#include <tuple>
#include <vector>
#include "env.hpp"

struct Texture {
  Texture(int width, int height);                             // RenderBuffer
  Texture(std::string filename);                              // Basic texture
  Texture(std::string filename, int offset_x, int offset_y);  // Texture array
  Texture(const std::vector<std::string>& textures);          // Cubemap
  ~Texture();
  std::string filename;

  GLuint id = 0;
  int height = 0;
  int width = 0;
};
