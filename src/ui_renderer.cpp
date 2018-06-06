#include "ui_renderer.hpp"

namespace render {
UiRenderer::UiRenderer(void) {
  /*
  glGenVertexArrays(1, &this->_vao);
  glGenBuffers(1, &this->_vbo);

  glBindVertexArray(this->_vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);*/
  std::vector<glm::vec4> dummy(6);
  _vao = new VAO(dummy);
}

UiRenderer::UiRenderer(UiRenderer const &src) { *this = src; }

UiRenderer::~UiRenderer(void) {
  auto it = _texture_cache.begin();
  while (it != _texture_cache.end()) {
    delete it->second;
    it++;
  }
  delete _vao;
}

UiRenderer &UiRenderer::operator=(UiRenderer const &rhs) {
  if (this != &rhs) {
  }
  return (*this);
}

void UiRenderer::renderUI(Shader *shader, std::string texture_name, float pos_x,
                          float pos_y, float scale, glm::mat4 ortho,
                          bool centered) {
  if (shader == nullptr) return;
  Texture *texture = nullptr;
  auto it = _texture_cache.find(texture_name);
  if (it == _texture_cache.end()) {
    try {
      texture = new Texture(texture_name);
      _texture_cache.emplace(texture_name, texture);
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << std::endl;
    }
  } else {
    texture = it->second;
  }
  if (texture == nullptr) return;
  glUseProgram(shader->id);
  glUniformMatrix4fv(glGetUniformLocation(shader->id, "proj"), 1, GL_FALSE,
                     glm::value_ptr(ortho));
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(_vao->vao);
  GLfloat w = texture->width * scale;
  GLfloat h = texture->height * scale;

  GLfloat xpos = pos_x;
  GLfloat ypos = pos_y;

  if (centered) {
    xpos -= w / 2;
    ypos -= h / 2;
  }
  pos_x *= scale;
  pos_y *= scale;
  std::vector<glm::vec4> vertices = {
      {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
      {xpos + w, ypos, 1.0, 1.0},

      {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
      {xpos + w, ypos + h, 1.0, 0.0}};
  glBindTexture(GL_TEXTURE_2D, texture->id);
  /*
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);*/
  _vao->update(vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
}  // namespace render
