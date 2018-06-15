#include "game.hpp"

Game::Game(void) {
  _camera =
      new Camera(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

  Model model = Model("data/sponza/sponza.obj");
  std::vector<std::string> albedo_textures;
  std::vector<std::string> normal_textures;
  std::vector<std::string> metallic_textures;
  std::vector<std::string> roughness_textures;
  for (const auto& mesh : model.meshes) {
    albedo_textures.push_back(mesh.diffuse_texname);
    normal_textures.push_back(mesh.bump_texname);
    metallic_textures.push_back(mesh.metallic_texname);
    roughness_textures.push_back(mesh.roughness_texname);
  }
  _albedo_array = new TextureArray(albedo_textures);
  _normal_array = new TextureArray(normal_textures);
  _metallic_array = new TextureArray(metallic_textures);
  _roughness_array = new TextureArray(roughness_textures);

  for (const auto& mesh : model.meshes) {
    std::vector<Vertex> vertices;
    for (size_t i = mesh.vertexOffset;
         i < model.vertices.size() && i < mesh.vertexOffset + mesh.indexCount;
         i++) {
      vertices.push_back(model.vertices[i]);
    }
    render::Attrib attrib;
    attrib.shader_key = "default";
    attrib.model = glm::scale(glm::vec3(0.01f));
    attrib.material = mesh.material;

    attrib.albedo = _albedo_array->getTextureIndex(mesh.diffuse_texname);
    attrib.normal = _normal_array->getTextureIndex(mesh.bump_texname);
    attrib.metallic = _metallic_array->getTextureIndex(mesh.metallic_texname);
    attrib.roughness =
        _roughness_array->getTextureIndex(mesh.roughness_texname);

    attrib.alpha_mask = mesh.alpha_mask;

    attrib.vao = new VAO(vertices);
    attribs.push_back(attrib);
  }
}

Game::Game(Game const& src) { *this = src; }

Game::~Game(void) {
  delete _camera;
  for (auto& attrib : attribs) {
    if (attrib.vao != nullptr) {
      delete attrib.vao;
    }
  }
  if (_albedo_array != nullptr) {
    delete _albedo_array;
  }
  if (_normal_array != nullptr) {
    delete _normal_array;
  }
  if (_metallic_array != nullptr) {
    delete _metallic_array;
  }
  if (_roughness_array != nullptr) {
    delete _roughness_array;
  }
}
Game& Game::operator=(Game const& rhs) {
  if (this != &rhs) {
    this->_debugMode = rhs._debugMode;
    this->_camera = new Camera(*rhs._camera);
  }
  return (*this);
}

void Game::update(Env& env) {
  _camera->update(env, env.getDeltaTime());
  if (env.inputHandler.keys[GLFW_KEY_I]) {
    env.inputHandler.keys[GLFW_KEY_I] = false;
    _debugMode = !_debugMode;
  }
}

void Game::render(const Env& env, render::Renderer& renderer) {
  float fwidth = static_cast<float>(renderer.getScreenWidth());
  float fheight = static_cast<float>(renderer.getScreenHeight());
  renderer.uniforms.albedo_array = _albedo_array;
  renderer.uniforms.normal_array = _normal_array;
  renderer.uniforms.metallic_array = _metallic_array;
  renderer.uniforms.roughness_array = _roughness_array;
  renderer.uniforms.view = _camera->view;
  renderer.uniforms.proj = _camera->proj;
  renderer.uniforms.view_proj = _camera->proj * _camera->view;
  renderer.uniforms.inv_view_proj = glm::inverse(renderer.uniforms.view_proj);
  renderer.uniforms.view_pos = _camera->pos;
  renderer.uniforms.time = env.getAbsoluteTime();
  renderer.uniforms.screen_size =
      glm::ivec2(renderer.getScreenWidth(), renderer.getScreenHeight());
  renderer.uniforms.debug = _debugMode ? 1 : 0;

  for (const auto& attrib : attribs) {
    renderer.addAttrib(attrib);
  }

  renderer.draw();

  renderer.flushAttribs();
  if (_debugMode) {
    print_debug_info(env, renderer, *_camera);
  }
}

std::string float_to_string(float f, int prec) {
  std::ostringstream out;
  out << std::setprecision(prec) << std::fixed << f;
  return out.str();
}

void Game::print_debug_info(const Env& env, render::Renderer& renderer,
                            Camera& camera) {
  float fheight = static_cast<float>(renderer.getScreenHeight());
  float fwidth = static_cast<float>(renderer.getScreenWidth());
  renderer.renderText(10.0f, fheight - 25.0f, 0.35f,
                      "x: " + float_to_string(camera.pos.x, 2) +
                          " y: " + float_to_string(camera.pos.y, 2) +
                          " z: " + float_to_string(camera.pos.z, 2),
                      glm::vec3(1.0f, 1.0f, 1.0f));
}
