#include "vao.hpp"

VAO::VAO(const std::vector<Vertex> &vertices) {
  genBuffers(vertices);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, normal));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, uv));
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, tangent));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

VAO::VAO(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices) {
  genBuffers(vertices, indices);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, normal));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, uv));
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, tangent));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

VAO::VAO(const std::vector<glm::vec2> &positions) {
  genBuffers(positions);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec2> &positions,
         const std::vector<unsigned int> &indices) {
  genBuffers(positions, indices);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}
VAO::VAO(const std::vector<glm::vec3> &positions) {
  genBuffers(positions);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec3> &positions,
         const std::vector<unsigned int> &indices) {
  genBuffers(positions, indices);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec4> &positions) {
  genBuffers(positions);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec4> &positions,
         const std::vector<unsigned int> &indices) {
  genBuffers(positions, indices);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::~VAO() {
  if (this->_vbo != 0) glDeleteBuffers(1, &this->_vbo);
  if (this->_ebo != 0) glDeleteBuffers(1, &this->_ebo);
  if (this->vao != 0) glDeleteVertexArrays(1, &this->vao);
}
