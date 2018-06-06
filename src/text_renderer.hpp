#pragma once
#include <map>
#include "anim.hpp"
#include "env.hpp"
#include "io.hpp"
#include "shader.hpp"
#include "vao.hpp"

namespace render {

struct TextProperties {
  glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
  std::string font_filename;
};

struct Character {
  GLuint textureID;
  glm::ivec2 size;
  glm::ivec2 bearing;
  GLuint advanceOffset;
};

struct Font {
  std::map<GLchar, Character> characters;
  float size;
};

class TextRenderer {
 public:
  TextRenderer(void);
  ~TextRenderer();

  void loadFont(std::string font_filename, float size = 48.0f);
  void update(const Env &env);
  void renderText(Shader *shader, float pos_x, float pos_y, float scale,
                  std::string text, TextProperties properties);

 private:
  glm::mat4 _ortho;
  std::map<std::string, Font> _font_map;
  // std::map<GLchar, Character> _characters;
  VAO *_vao = nullptr;
};
}  // namespace render
