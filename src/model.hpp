#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "forward.hpp"

class Mesh {
 public:
  Mesh(uint32_t count, int32_t offset);
  ~Mesh();
  Mesh(Mesh const& src);
  Mesh& operator=(Mesh const& rhs);

  uint32_t indexCount;   // vertices count
  int32_t vertexOffset;  // offset in vertex array
  Material material;
  std::string ambient_texname;
  std::string diffuse_texname;
  std::string specular_texname;
  std::string bump_texname;

 private:
  Mesh();
};

class Model {
 public:
  Model(const std::string filename);
  ~Model();

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Mesh> meshes;
};

std::string sanitizeFilename(std::string filename);
std::string getBaseDir(const std::string& filepath);
