#include "text_renderer.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
namespace render {

TextRenderer::TextRenderer(void) {
  loadFont("fonts/minecraft.ttf");
  std::vector<glm::vec4> dummy(6);
  _vao = new VAO(dummy);
}

void TextRenderer::loadFont(std::string font_filename, float size) {
  Font font;
  font.size = size;
  auto font_it = _font_map.find(font_filename);
  if (font_it != _font_map.end()) {
    std::cerr << font_filename << " already loaded" << std::endl;
  }
  unsigned int filesize = io::get_filesize(font_filename);
  FILE *font_fp = fopen(font_filename.c_str(), "rb");
  if (font_fp == nullptr) {
    return;
  }
  unsigned char *ttf_buffer = new unsigned char[filesize + 1];
  fread(ttf_buffer, filesize + 1, 1, font_fp);

  int font_offset = stbtt_GetFontOffsetForIndex(ttf_buffer, 0);
  if (font_offset == -1) {
    std::cout << "wrong font offset" << std::endl;
    return;
  }
  stbtt_fontinfo font_info;
  if (stbtt_InitFont(&font_info, ttf_buffer, font_offset) == 0) {
    std::cout << "Failed to load font" << std::endl;
    return;
  }
  float font_scale = stbtt_ScaleForPixelHeight(&font_info, size);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (GLubyte c = 0; c < 128; c++) {
    int w, h, xoff, yoff;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(&font_info, 0, font_scale,
                                                     c, &w, &h, &xoff, &yoff);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE,
                 bitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int advance_width;
    stbtt_GetCodepointHMetrics(&font_info, c, &advance_width, 0);
    Character character = {texture, glm::ivec2(w, h), glm::ivec2(xoff, -yoff),
                           static_cast<GLuint>(advance_width * font_scale)};
    font.characters.insert(std::pair<GLchar, Character>(c, character));
    stbtt_FreeBitmap(bitmap, NULL);
  }

  fclose(font_fp);
  delete[] ttf_buffer;
  _font_map.emplace(font_filename, font);
}

TextRenderer::~TextRenderer() {
  for (auto &font : _font_map) {
    for (auto &character : font.second.characters) {
      glDeleteTextures(1, &character.second.textureID);
    }
  }
  delete _vao;
}
void TextRenderer::update(const Env &env) {
  _ortho = glm::ortho(0.0f, static_cast<float>(env.width), 0.0f,
                      static_cast<float>(env.height));
}

void TextRenderer::renderText(Shader *shader, float pos_x, float pos_y,
                              float scale, std::string text,
                              TextProperties properties) {
  if (shader == nullptr) return;
  Font font;

  if (properties.font_filename.empty()) {
    if (_font_map.empty()) {
      std::cerr << "Cannot render text, no font were loaded" << std::endl;
      return;
    }
    font = _font_map.begin()->second;
  } else {
    auto font_it = _font_map.find(properties.font_filename);
    if (font_it == _font_map.end()) {
      loadFont(properties.font_filename);
      font_it = _font_map.find(properties.font_filename);
      if (font_it == _font_map.end()) {
        std::cerr << "Cannot load " << properties.font_filename
                  << " , fallback on default font" << std::endl;
        properties.font_filename = "";
        renderText(shader, pos_x, pos_y, scale, text, properties);
        return;
      }
      font = font_it->second;
    }
  }
  glUseProgram(shader->id);
  glUniformMatrix4fv(glGetUniformLocation(shader->id, "P"), 1, GL_FALSE,
                     glm::value_ptr(_ortho));
  glUniform3fv(glGetUniformLocation(shader->id, "color"), 1,
               glm::value_ptr(properties.color));
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(_vao->vao);
  for (char &c : text) {
    if (static_cast<int>(c) > 0 && static_cast<int>(c) < 128) {
      Character ch = font.characters[static_cast<int>(c)];
      GLfloat xpos = pos_x + ch.bearing.x * scale;
      GLfloat ypos = pos_y - (ch.size.y - ch.bearing.y) * scale;

      GLfloat w = ch.size.x * scale;
      GLfloat h = ch.size.y * scale;
      std::vector<glm::vec4> vertices = {
          {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
          {xpos + w, ypos, 1.0, 1.0},

          {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
          {xpos + w, ypos + h, 1.0, 0.0}};
      glBindTexture(GL_TEXTURE_2D, ch.textureID);
      _vao->update(vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      pos_x += ch.advanceOffset * scale;
    }
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
}  // namespace render
