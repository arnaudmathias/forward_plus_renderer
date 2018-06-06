#pragma once
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include "anim.hpp"
#include "env.hpp"
#include "io.hpp"
#include "shader.hpp"
#include "shader_cache.hpp"
#include "text_renderer.hpp"
#include "texture.hpp"
#include "ui_renderer.hpp"
#include "vao.hpp"

class Shader;

namespace render {

enum class DepthTestFunc {
  Less,
  Never,
  Equal,
  LessEqual,
  Greater,
  NotEqual,
  GreaterEqual,
  Always
};

enum class PrimitiveMode {
  Points,
  LineStrip,
  LineLoop,
  Lines,
  LineStripAdjacency,
  LinesAdjacency,
  TriangleStrip,
  TriangleFan,
  Triangles,
  TriangleStripAdjacency,
  TriananglesAdjacency,
  Patches
};

enum class BlendFunc {
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  DstColor,
  OneMinusDstColor,
  SrcApha,
  OneMinusSrcAlpha,
  DstAlpha,
  OneMinusDstAlpha,
  ConstantColor,
  OneMinusConstantColor,
  ConstantAlpha,
  OneMinusConstantAlpha
};

enum class PolygonMode { Point, Line, Fill };

struct RenderState {
  enum PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
  enum PolygonMode polygonMode = PolygonMode::Fill;
  enum DepthTestFunc depthTestFunc = DepthTestFunc::Less;
  enum BlendFunc blendFunc = BlendFunc::Zero;
  bool depthTest = true;
  bool blending = true;
};

struct Uniforms {
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 ortho;
  glm::mat4 view_proj;
};

struct Attrib {
  glm::mat4 model;
  std::string shader_key = "";
  std::vector<VAO*> vaos;
  std::vector<glm::mat4> bones;

  RenderState state;
  Attrib(){};

  bool operator<(const struct Attrib& rhs) const;
};

class Renderer {
 public:
  Renderer(int width, int height);
  Renderer(Renderer const& src);
  ~Renderer(void);
  Renderer& operator=(Renderer const& rhs);
  void addAttrib(const Attrib& attrib);
  void renderText(float pos_x, float pos_y, float scale, std::string text,
                  glm::vec3 color);
  void renderUI(std::string filename, float pos_x, float pos_y, float scale,
                bool centered);
  void update(const Env& env);
  void draw();
  void flushAttribs();
  int getScreenWidth();
  int getScreenHeight();
  void loadCubeMap(std::string vertex_sha, std::string fragment_sha,
                   const std::vector<std::string>& textures);
  void clearScreen();

  void setState(const RenderState& new_state);
  void switchPolygonMode(enum PolygonMode mode);
  void switchDepthTestFunc(enum DepthTestFunc mode);
  void switchBlendingFunc(enum BlendFunc mode);

  void switchDepthTestState(bool state);
  void switchBlendingState(bool state);

  Uniforms uniforms;

 private:
  Renderer(void) = default;
  std::vector<Attrib> _attribs;
  int _width = 0;
  int _height = 0;
  Texture* _cubeMapTexture = nullptr;
  VAO* _cubeMapVao = nullptr;
  ShaderCache _shaderCache;

  RenderState _state;

  TextRenderer _textRenderer;
  UiRenderer _uiRenderer;

  void switchShader(GLuint shader_id, int& current_shader_id);
  void updateUniforms(const Attrib& attrib, const int shader_id);
  GLenum getGLRenderMode(enum PrimitiveMode mode);
};

static inline void setUniform(const GLint& location, const float& data) {
  glUniform1f(location, data);
}
static inline void setUniform(const GLint& location, const int& data) {
  glUniform1i(location, data);
}
static inline void setUniform(const GLint& location, const glm::vec2& data) {
  glUniform2fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::vec3& data) {
  glUniform3fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::vec4& data) {
  glUniform4fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::mat4& data) {
  glUniformMatrix4fv(location, 1, GL_FALSE,
                     static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location,
                              const std::vector<glm::mat4>& data) {
  if (data.size() > 0) {
    glUniformMatrix4fv(location, data.size(), GL_FALSE,
                       static_cast<const GLfloat*>(glm::value_ptr(data[0])));
  }
}
}  // namespace render
