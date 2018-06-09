#include "model.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh() {}

Mesh::Mesh(uint32_t count, int32_t offset)
    : indexCount(count), vertexOffset(offset) {}

Mesh::~Mesh() {}

Mesh::Mesh(Mesh const& src) { *this = src; }
Mesh& Mesh::operator=(Mesh const& rhs) {
  if (this != &rhs) {
    indexCount = rhs.indexCount;
    vertexOffset = rhs.indexCount;
    material = rhs.material;
    ambient_texname = rhs.ambient_texname;
    diffuse_texname = rhs.diffuse_texname;
    specular_texname = rhs.specular_texname;
    bump_texname = rhs.bump_texname;
  }
  return (*this);
}

Model::Model(const std::string filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  std::string basedir = getBaseDir(filename);
  if (basedir.empty()) basedir = ".";
  basedir += "/";

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(),
                        basedir.c_str())) {
    std::cerr << "Cannot load obj: " << filename.c_str() << std::endl;
    return;
  }
  if (materials.empty()) {
    materials.push_back(tinyobj::material_t());
  }
  for (const auto& material : materials) {
    Mesh mesh(0, 0);

    // Copy material from tinyobjloader material_t
    mesh.material.ambient = glm::vec4(material.ambient[0], material.ambient[1],
                                      material.ambient[2], 1.0f);
    mesh.material.diffuse = glm::vec4(material.diffuse[0], material.diffuse[1],
                                      material.diffuse[2], 1.0f);
    mesh.material.specular = glm::vec4(
        material.specular[0], material.specular[1], material.specular[2], 1.0f);
    mesh.material.transmittance =
        glm::vec4(material.transmittance[0], material.transmittance[1],
                  material.transmittance[2], 1.0f);
    mesh.material.emission = glm::vec4(
        material.emission[0], material.emission[1], material.emission[2], 1.0f);

    mesh.material.opacity = material.dissolve;
    mesh.material.index_of_refraction = material.ior;
    mesh.material.specular_power = material.shininess;

    mesh.ambient_texname = sanitizeFilename(basedir + material.ambient_texname);
    mesh.diffuse_texname = sanitizeFilename(basedir + material.diffuse_texname);
    mesh.specular_texname =
        sanitizeFilename(basedir + material.specular_texname);
    mesh.specular_highlight_texname =
        sanitizeFilename(basedir + material.specular_highlight_texname);
    mesh.bump_texname = sanitizeFilename(basedir + material.bump_texname);
    mesh.displacement_texname =
        sanitizeFilename(basedir + material.displacement_texname);
    mesh.alpha_texname = sanitizeFilename(basedir + material.alpha_texname);

    mesh.roughness_texname =
        sanitizeFilename(basedir + material.roughness_texname);
    mesh.metallic_texname =
        sanitizeFilename(basedir + material.metallic_texname);
    mesh.sheen_texname = sanitizeFilename(basedir + material.sheen_texname);
    mesh.emissive_texname =
        sanitizeFilename(basedir + material.emissive_texname);
    mesh.normal_texname = sanitizeFilename(basedir + material.normal_texname);

    mesh.material.roughness = material.roughness;
    mesh.material.metallic = material.metallic;
    mesh.material.sheen = material.sheen;
    mesh.material.clearcoat_thickness = material.clearcoat_thickness;
    mesh.material.clearcoat_roughness = material.clearcoat_roughness;
    mesh.material.anisotropy = material.anisotropy;
    mesh.material.anisotropy_rotation = material.anisotropy_rotation;

    meshes.push_back(mesh);
  }

  struct Face {
    Vertex vertices[3];
    int32_t material_id;
  };

  std::vector<Face> faceList;
  for (const auto& shape : shapes) {
    for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
      Face face;

      int material_id;
      material_id =
          f < shape.mesh.material_ids.size() ? shape.mesh.material_ids[f] : 0;
      if (material_id == -1) material_id = 0;
      face.material_id = material_id;
      bool normalNeeded = true;
      for (size_t j = 0; j < 3; j++) {
        int vertex_index = shape.mesh.indices[(f * 3) + j].vertex_index;
        int normal_index = shape.mesh.indices[(f * 3) + j].normal_index;
        int texcoord_index = shape.mesh.indices[(f * 3) + j].texcoord_index;

        if (vertex_index != -1) {
          face.vertices[j].position = {attrib.vertices[3 * vertex_index + 0],
                                       attrib.vertices[3 * vertex_index + 1],
                                       attrib.vertices[3 * vertex_index + 2]};
        }
        if (texcoord_index != -1) {
          face.vertices[j].uv = {attrib.texcoords[2 * texcoord_index + 0],
                                 attrib.texcoords[2 * texcoord_index + 1]};
        }
        if (normal_index != -1) {
          face.vertices[j].normal = {attrib.normals[3 * normal_index + 0],
                                     attrib.normals[3 * normal_index + 1],
                                     attrib.normals[3 * normal_index + 2]};
          face.vertices[j].normal = glm::normalize(face.vertices[j].normal);
          normalNeeded = false;
        }
      }
      if (normalNeeded) {
        glm::vec3 normal = glm::normalize(
            glm::cross(face.vertices[2].position - face.vertices[0].position,
                       face.vertices[1].position - face.vertices[0].position));
        face.vertices[0].normal = normal;
        face.vertices[1].normal = normal;
        face.vertices[2].normal = normal;
      }

      glm::vec3 edge1 = face.vertices[1].position - face.vertices[0].position;
      glm::vec3 edge2 = face.vertices[2].position - face.vertices[0].position;
      glm::vec2 deltaUV1 = face.vertices[1].uv - face.vertices[0].uv;
      glm::vec2 deltaUV2 = face.vertices[2].uv - face.vertices[0].uv;

      float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

      glm::vec3 tangent;
      tangent.x = r * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
      tangent.y = r * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
      tangent.z = r * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
      tangent = glm::normalize(tangent);
      face.vertices[0].tangent = tangent;
      face.vertices[1].tangent = tangent;
      face.vertices[2].tangent = tangent;

      faceList.push_back(face);
    }
  }
  // Sort vertices by material
  std::cout << "material_count: " << materials.size() << std::endl;
  for (size_t material_id = 0; material_id < materials.size(); material_id++) {
    int vertexCount = 0;
    meshes[material_id].vertexOffset = vertices.size();
    for (const auto& face : faceList) {
      if (face.material_id == material_id) {
        vertices.push_back(face.vertices[0]);
        vertices.push_back(face.vertices[1]);
        vertices.push_back(face.vertices[2]);
        indices.push_back(indices.size());
        indices.push_back(indices.size());
        indices.push_back(indices.size());
        vertexCount += 3;
      }
    }
    meshes[material_id].indexCount = vertexCount;
  }
}

Model::~Model() {}

std::string sanitizeFilename(std::string filename) {
  std::replace(filename.begin(), filename.end(), '\\', '/');
  return (filename);
}

std::string getBaseDir(const std::string& filepath) {
  if (filepath.find_last_of("/\\") != std::string::npos)
    return filepath.substr(0, filepath.find_last_of("/\\"));
  return "";
}
