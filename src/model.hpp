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

  std::string ambient_texname;
  std::string diffuse_texname;
  std::string specular_texname;
  std::string specular_highlight_texname;
  std::string bump_texname;
  std::string displacement_texname;
  std::string alpha_texname;

  // PBR textures
  std::string roughness_texname;
  std::string metallic_texname;
  std::string sheen_texname;
  std::string emissive_texname;
  std::string normal_texname;

  Material material;

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
