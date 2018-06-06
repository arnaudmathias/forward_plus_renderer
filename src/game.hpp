#pragma once
#include <iomanip>
#include "anim.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "renderer.hpp"

class Game {
 public:
  Game(void);
  Game(Game const& src);
  virtual ~Game(void);
  Game& operator=(Game const& rhs);
  void update(Env& env);
  void render(const Env& env, render::Renderer& renderer);

 private:
  bool _debugMode = false;
  Camera* _camera;
  std::vector<Model*> _models;

  void print_debug_info(const Env& env, render::Renderer& renderer,
                        Camera& camera);
};
