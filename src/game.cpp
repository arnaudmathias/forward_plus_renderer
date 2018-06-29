#include "game.hpp"

Game::Game(void) {
  _camera = std::make_unique<Camera>(glm::vec3(-6.0f, -5.0f, 0.0f),
                                     glm::vec3(-5.0f, -5.0f, 0.0f));

  Model* model = new Model("data/sponza/sponza.obj");
  glm::mat4 scene_scale = glm::scale(glm::vec3(0.01f));
  glm::mat4 scene_transform = glm::translate(
      -glm::vec3(scene_scale * glm::vec4(model->aabb_center, 1.0f)));

  glm::mat4 scene_model = scene_transform * scene_scale;
  scene_aabb_center =
      glm::vec3(scene_model * glm::vec4(model->aabb_center, 1.0f));
  scene_aabb_halfsize =
      glm::vec3(scene_model * glm::vec4(model->aabb_halfsize, 0.0f));

  std::vector<std::string> albedo_textures;
  std::vector<std::string> normal_textures;
  std::vector<std::string> metallic_textures;
  std::vector<std::string> roughness_textures;
  for (const auto& mesh : model->meshes) {
    albedo_textures.push_back(mesh.diffuse_texname);
    normal_textures.push_back(mesh.bump_texname);
    metallic_textures.push_back(mesh.metallic_texname);
    roughness_textures.push_back(mesh.roughness_texname);
  }
  _albedo_array = std::make_shared<TextureArray>(albedo_textures);
  _normal_array = std::make_shared<TextureArray>(normal_textures);
  _metallic_array = std::make_shared<TextureArray>(metallic_textures);
  _roughness_array = std::make_shared<TextureArray>(roughness_textures);

  for (const auto& mesh : model->meshes) {
    std::vector<Vertex> vertices;
    for (size_t i = mesh.vertexOffset;
         i < model->vertices.size() && i < mesh.vertexOffset + mesh.indexCount;
         i++) {
      vertices.push_back(model->vertices[i]);
    }
    render::Attrib attrib;
    attrib.model = scene_model;
    attrib.material = mesh.material;

    attrib.albedo_index = _albedo_array->getTextureIndex(mesh.diffuse_texname);
    attrib.normal_index = _normal_array->getTextureIndex(mesh.bump_texname);
    attrib.metallic_index =
        _metallic_array->getTextureIndex(mesh.metallic_texname);
    attrib.roughness_index =
        _roughness_array->getTextureIndex(mesh.roughness_texname);

    attrib.alpha_mask = mesh.alpha_mask;

    attrib.vao = std::make_shared<VAO>(vertices);
    attribs.push_back(attrib);
  }
  delete model;
  glm::vec3 min_bound = scene_aabb_center - scene_aabb_halfsize;
  glm::vec3 max_bound = scene_aabb_center + scene_aabb_halfsize;
  min_bound = glm::vec3(min_bound.x + 3.0f, min_bound.y, min_bound.z + 3.0f);
  max_bound = glm::vec3(max_bound.x - 3.0f, max_bound.y, max_bound.z - 3.0f);
  for (int i = 0; i < NUM_LIGHTS; i++) {
    // lights.lights[i].position = glm::vec3(-10.0 + i * 10.0, -6.0f, 0.0f);
    lights.lights[i].position =
        glm::vec3(glm::linearRand(min_bound.x, max_bound.x), min_bound.y,
                  glm::linearRand(min_bound.z, max_bound.z));
    lights.lights[i].radius = 4.0f;
    lights.lights[i].color =
        glm::vec3(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f),
                  glm::linearRand(0.0f, 1.0f));
    lights.lights[i].intensity = 1.0f;
    lights_speed[i] = glm::linearRand(0.5f, 5.0f);
  }
}

Game::Game(Game const& src) { *this = src; }

Game::~Game(void) {}

Game& Game::operator=(Game const& rhs) {
  if (this != &rhs) {
    _debug_mode = rhs._debug_mode;
    _static_light_mode = rhs._static_light_mode;
    _camera = std::make_unique<Camera>(*rhs._camera);
    _albedo_array = std::make_shared<TextureArray>(*rhs._albedo_array);
    _normal_array = std::make_shared<TextureArray>(*rhs._normal_array);
    _metallic_array = std::make_shared<TextureArray>(*rhs._metallic_array);
    _roughness_array = std::make_shared<TextureArray>(*rhs._roughness_array);
  }
  return (*this);
}

void Game::update(Env& env) {
  _camera->update(env, env.getDeltaTime());
  glm::vec3 min_bound = scene_aabb_center - scene_aabb_halfsize;
  glm::vec3 max_bound = scene_aabb_center + scene_aabb_halfsize;
  min_bound = glm::vec3(min_bound.x + 3.0f, min_bound.y, min_bound.z + 3.0f);
  max_bound = glm::vec3(max_bound.x - 3.0f, max_bound.y, max_bound.z - 3.0f);
  for (int i = 0; i < NUM_LIGHTS; i++) {
    if (lights.lights[i].position.y > max_bound.y + 3.0f) {
      lights.lights[i].position =
          glm::vec3(glm::linearRand(min_bound.x, max_bound.x), min_bound.y,
                    glm::linearRand(min_bound.z, max_bound.z));
      lights.lights[i].radius = 2.0f;
      lights.lights[i].color =
          glm::vec3(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f),
                    glm::linearRand(0.0f, 1.0f));
      lights_speed[i] = glm::linearRand(0.5f, 5.0f);
    } else if (_static_light_mode == false) {
      lights.lights[i].position.y += lights_speed[i] * env.getDeltaTime();
    }
  }
  if (env.inputHandler.keys[GLFW_KEY_SPACE]) {
    env.inputHandler.keys[GLFW_KEY_SPACE] = false;
    _static_light_mode = !_static_light_mode;
  }
  if (env.inputHandler.keys[GLFW_KEY_I]) {
    env.inputHandler.keys[GLFW_KEY_I] = false;
    _debug_mode = !_debug_mode;
  }
  if (env.inputHandler.keys[GLFW_KEY_Q]) {
    env.inputHandler.keys[GLFW_KEY_Q] = false;
    _light_debug_mode = !_light_debug_mode;
  }
  if (env.inputHandler.keys[GLFW_KEY_E]) {
    env.inputHandler.keys[GLFW_KEY_E] = false;
    _visibilty_debug_mode = !_visibilty_debug_mode;
  }
}

void Game::render(const Env& env, render::Renderer& renderer) {
  float fwidth = static_cast<float>(renderer.getScreenWidth());
  float fheight = static_cast<float>(renderer.getScreenHeight());
  renderer.uniforms.lights = lights;
  renderer.uniforms.albedo_array = _albedo_array;
  renderer.uniforms.normal_array = _normal_array;
  renderer.uniforms.metallic_array = _metallic_array;
  renderer.uniforms.roughness_array = _roughness_array;
  renderer.uniforms.view = _camera->view;
  renderer.uniforms.proj = _camera->proj;
  renderer.uniforms.inv_proj = glm::inverse(_camera->proj);
  renderer.uniforms.view_proj = _camera->proj * _camera->view;
  renderer.uniforms.view_pos = _camera->pos;
  renderer.uniforms.time = env.getAbsoluteTime();
  renderer.uniforms.screen_size =
      glm::ivec2(renderer.getScreenWidth(), renderer.getScreenHeight());
  renderer.uniforms.debug = _debug_mode ? 1 : 0;
  renderer.uniforms.light_debug = _light_debug_mode ? 1 : 0;
  renderer.uniforms.visibilty_debug = _visibilty_debug_mode ? 1 : 0;

  for (const auto& attrib : attribs) {
    renderer.addAttrib(attrib);
  }

  renderer.draw();

  renderer.flushAttribs();
  if (_debug_mode) {
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
  renderer.renderText(fwidth - 120.0f, fheight - 25.0f, 0.35f,
                      float_to_string(env.getFPS(), 2) + " fps",
                      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 25.0f, 0.35f,
                      std::to_string(env.width) + " x " +
                          std::to_string(env.height) + " (" +
                          std::to_string(env.getFrame()) + ")",
                      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 50.0f, 0.35f,
                      "x: " + float_to_string(camera.pos.x, 2) +
                          " y: " + float_to_string(camera.pos.y, 2) +
                          " z: " + float_to_string(camera.pos.z, 2),
                      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 75.0f, 0.35f,
                      std::to_string(NUM_LIGHTS) + " lights",
                      glm::vec3(1.0f, 1.0f, 1.0f));
}
