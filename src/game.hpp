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
  bool _debugMode = false;
  Camera* _camera = nullptr;
  TextureArray* _texture_array = nullptr;
  std::vector<VAO*> vaos;
  std::vector<render::Attrib> attribs;

  void print_debug_info(const Env& env, render::Renderer& renderer,
                        Camera& camera);
};
