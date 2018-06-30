#pragma once
#include <algorithm>
#include <vector>
#include "env.hpp"
#include "forward.hpp"

class VAO {
 public:
  VAO(const std::vector<Vertex>& vertices);
  VAO(const std::vector<Vertex>& vertices,
      const std::vector<unsigned int>& indices);

  VAO(const std::vector<glm::vec2>& positions);
  VAO(const std::vector<glm::vec2>& positions,
      const std::vector<unsigned int>& indices);

  VAO(const std::vector<glm::vec3>& positions);
  VAO(const std::vector<glm::vec3>& positions,
      const std::vector<unsigned int>& indices);

  VAO(const std::vector<glm::vec4>& positions);
  VAO(const std::vector<glm::vec4>& positions,
      const std::vector<unsigned int>& indices);

  VAO(VAO const&) = delete;
  VAO& operator=(VAO const& rhs) = delete;

  VAO(VAO&&) noexcept = default;
  VAO& operator=(VAO&&) noexcept = default;

  ~VAO();

  template <typename T>
  void update(
      const std::vector<T>& vertices,
      const std::vector<unsigned int>& indices = std::vector<unsigned int>()) {
    this->vertices_size = vertices.size();
    if (vertices.size() > 0) {
      glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T),
                   vertices.data(), GL_STATIC_DRAW);
    }
    this->indices_size = indices.size();
    if (indices.size() > 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   this->indices_size * sizeof(unsigned int), indices.data(),
                   GL_STATIC_DRAW);
    }
  }

  GLuint vao = 0;
  GLsizei vertices_size = 0;
  GLsizei indices_size = 0;

 private:
  GLuint _vbo = 0;
  GLuint _ebo = 0;

  template <typename T>
  GLuint genBuffer(GLenum type, const std::vector<T>& buffer) {
    GLuint bo = 0;
    if (buffer.size() > 0) {
      glGenBuffers(1, &bo);
      glBindBuffer(type, bo);
      glBufferData(type, buffer.size() * sizeof(T), buffer.data(),
                   GL_STATIC_DRAW);
      glBindBuffer(type, 0);
    }
    return (bo);
  }

  template <typename T>
  void genBuffers(
      const std::vector<T>& vertices,
      const std::vector<unsigned int>& indices = std::vector<unsigned int>()) {
    vertices_size = vertices.size();
    indices_size = indices.size();
    _vbo = genBuffer(GL_ARRAY_BUFFER, vertices);
    _ebo = genBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
  }
};
