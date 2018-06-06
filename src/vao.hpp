#pragma once
#include <algorithm>
#include <vector>
#include "anim.hpp"
#include "env.hpp"

struct VAO {
  VAO(const std::vector<Vertex>& vertices);
  VAO(const std::vector<Vertex>& vertices,
      const std::vector<unsigned int>& indices);

  VAO(const std::vector<glm::vec3>& positions);
  VAO(const std::vector<glm::vec3>& positions,
      const std::vector<unsigned int>& indices);

  VAO(const std::vector<glm::vec4>& positions);
  VAO(const std::vector<glm::vec4>& positions,
      const std::vector<unsigned int>& indices);

  ~VAO();

  template <typename T>
  void update(const std::vector<T>& vertices) {
    this->vertices_size = vertices.size();
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(),
                 GL_STATIC_DRAW);
  }
  template <typename T>
  void update(const std::vector<T>& vertices,
              const std::vector<unsigned int> indices) {
    update(vertices);
    this->indices_size = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->indices_size * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);
  }

  GLuint vao;
  GLsizei vertices_size;
  GLsizei indices_size;

 private:
  void initialize();
  GLuint _vbo;
  GLuint _ebo;
};
