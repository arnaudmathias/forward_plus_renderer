#pragma once
#include <iomanip>
#include <memory>
#include "camera.hpp"
#include "forward.hpp"
#include "model.hpp"
#include "renderer.hpp"

class Game {
 public:
  Game(void);
  Game(Game const& src);
  ~Game(void);
  Game& operator=(Game const& rhs);
  void update(Env& env);
  void render(const Env& env, render::Renderer& renderer);

 private:
  bool _debug_mode = false;
  bool _light_debug_mode = false;
  bool _visibilty_debug_mode = false;
  bool _static_light_mode = false;
  std::unique_ptr<Camera> _camera;
  Lights lights;
  float lights_speed[NUM_LIGHTS] = {};

  std::shared_ptr<TextureArray> _albedo_array;
  std::shared_ptr<TextureArray> _normal_array;
  std::shared_ptr<TextureArray> _metallic_array;
  std::shared_ptr<TextureArray> _roughness_array;

  glm::vec3 scene_aabb_center;
  glm::vec3 scene_aabb_halfsize;

  std::vector<render::Attrib> attribs;

  void print_debug_info(const Env& env, render::Renderer& renderer,
                        Camera& camera);
};
