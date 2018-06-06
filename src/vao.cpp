#include "vao.hpp"

VAO::VAO(const std::vector<Vertex> &vertices) {
  initialize();
  this->vertices_size = vertices.size();
  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, normal));
  glVertexAttribIPointer(2, 4, GL_INT, sizeof(Vertex),
                         (GLvoid *)offsetof(Vertex, bone_ids));
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, weights));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

VAO::VAO(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices) {
  initialize();
  this->vertices_size = vertices.size();
  this->indices_size = indices.size();
  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &this->_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               this->indices_size * sizeof(unsigned int), indices.data(),
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, normal));
  glVertexAttribIPointer(2, 4, GL_INT, sizeof(Vertex),
                         (GLvoid *)offsetof(Vertex, bone_ids));
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, weights));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

VAO::VAO(const std::vector<glm::vec3> &positions) {
  initialize();
  this->vertices_size = positions.size();

  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(glm::vec3),
               positions.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec3> &positions,
         const std::vector<unsigned int> &indices) {
  initialize();
  this->vertices_size = positions.size();
  this->indices_size = indices.size();

  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(glm::vec3),
               positions.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &this->_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               this->indices_size * sizeof(unsigned int), indices.data(),
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec4> &positions) {
  initialize();
  this->vertices_size = positions.size();

  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(glm::vec4),
               positions.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::VAO(const std::vector<glm::vec4> &positions,
         const std::vector<unsigned int> &indices) {
  initialize();
  this->vertices_size = positions.size();

  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(glm::vec4),
               positions.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &this->_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               this->indices_size * sizeof(unsigned int), indices.data(),
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);

  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ebo);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

void VAO::initialize() {
  this->_vbo = 0;
  this->_ebo = 0;
  this->vao = 0;
  this->vertices_size = 0;
  this->indices_size = 0;
}

VAO::~VAO() {
  if (this->_vbo != 0) glDeleteBuffers(1, &this->_vbo);
  if (this->_ebo != 0) glDeleteBuffers(1, &this->_ebo);
  if (this->vao != 0) glDeleteVertexArrays(1, &this->vao);
}
