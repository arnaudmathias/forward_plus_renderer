#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "forward.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vao.hpp"

namespace render {

class UiRenderer {
 public:
  UiRenderer(void);
  UiRenderer(UiRenderer const& src);
  ~UiRenderer(void);
  UiRenderer& operator=(UiRenderer const& rhs);
  void renderUI(std::shared_ptr<Shader> shader, std::string texture_name,
                float pos_x, float pos_y, float scale, glm::mat4 ortho,
                bool centered);

 private:
  glm::mat4 ortho;
  std::unordered_map<std::string, Texture*> _texture_cache;
  std::unique_ptr<VAO> _vao;
};
}  // namespace render
