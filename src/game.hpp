#pragma once
#include <iomanip>
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
  bool _static_light_mode = false;
  Camera* _camera = nullptr;
  Lights lights;
  float lights_speed[NUM_LIGHTS] = {};

  TextureArray* _albedo_array = nullptr;
  TextureArray* _normal_array = nullptr;
  TextureArray* _metallic_array = nullptr;
  TextureArray* _roughness_array = nullptr;

  glm::vec3 scene_aabb_center;
  glm::vec3 scene_aabb_halfsize;

  std::vector<render::Attrib> attribs;

  void print_debug_info(const Env& env, render::Renderer& renderer,
                        Camera& camera);
};
