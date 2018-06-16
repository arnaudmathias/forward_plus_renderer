#pragma once
#include <algorithm>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>
#include "env.hpp"
#include "forward.hpp"
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

struct UBO {
  struct Material material = {};
};

struct Uniforms {
  Lights lights;
  TextureArray* albedo_array;
  TextureArray* normal_array;
  TextureArray* metallic_array;
  TextureArray* roughness_array;
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 ortho;
  glm::mat4 view_proj;
  glm::mat4 inv_view_proj;
  glm::ivec2 screen_size;
  glm::vec3 view_pos;
  float time = 0;
  int debug = 0;
};

struct Attrib {
  glm::mat4 model = glm::mat4(1.0f);
  std::string shader_key = "";
  VAO* vao = nullptr;
  Material material;

  int albedo = -1;
  int normal = -1;
  int metallic = -1;
  int roughness = -1;
  int opacity = -1;

  bool alpha_mask = false;
  RenderState state;

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
  void bindTexture(GLuint texture_id, GLenum tex_slot);
  void update(const Env& env);
  void draw();
  void flushAttribs();
  int getScreenWidth();
  int getScreenHeight();
  void clearScreen();

  void setState(const RenderState& new_state);
  void switchPolygonMode(enum PolygonMode mode);
  void switchDepthTestFunc(enum DepthTestFunc mode);
  void switchBlendingFunc(enum BlendFunc mode);

  void switchDepthTestState(bool state);
  void switchBlendingState(bool state);

  Uniforms uniforms = {};
  UBO ubo = {};
  GLuint ubo_id = 0;

  GLuint depthpass_fbo = 0;
  GLuint depthpass_texture_depth_id = 0;

  GLuint lightpass_fbo = 0;
  GLuint lightpass_texture_hdr_id = 0;
  GLuint lightpass_texture_normal_id = 0;
  GLuint lightpass_texture_depth_id = 0;

  GLuint ubo_lights = 0;

 private:
  Renderer(void) = default;
  std::vector<Attrib> _attribs;
  int _width = 0;
  int _height = 0;
  ShaderCache _shaderCache;
  VAO* _vao_quad = nullptr;
  VAO* _vao_octahedron = nullptr;

  RenderState _state;

  TextRenderer _textRenderer;
  UiRenderer _uiRenderer;

  void updateRessources();
  void drawVAOs(VAO* vao, PrimitiveMode primitive_mode);
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
static inline void setUniform(const GLint& location, const unsigned int& data) {
  glUniform1ui(location, data);
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
