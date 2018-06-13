#pragma once
#include <assert.h>
#include <cstring>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <vector>
#include "env.hpp"

struct Texture {
  Texture(std::string filename);                              // Basic texture
  Texture(std::string filename, int offset_x, int offset_y);  // Texture array
  Texture(const std::vector<std::string>& textures);          // Cubemap
  ~Texture();
  std::string filename;

  GLuint id = 0;
  int height = 0;
  int width = 0;
};

struct TextureArray {
  TextureArray(const std::vector<std::string>& textures);
  ~TextureArray();
  int getTextureIndex(std::string texture_name);

  GLuint id = 0;
  int height = 0;
  int width = 0;

 private:
  std::map<std::string, int> _lookup_table;
};
