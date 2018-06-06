#include "scene.hpp"

std::vector<Vertex> shader_plane_vertices = {
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}};

Scene::Scene(void) : _renderer(nullptr), _paused(false), _ended(false) {}

Scene::Scene(Renderer* renderer)
    : _renderer(renderer), _paused(false), _ended(false) {
  this->_camera =
      new Camera(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		 renderer->getScreenWidth(), renderer->getScreenHeight());
  this->_shader = loadShader(ShaderType::NORMAL, "shaders/pixels.vert",
			     "shaders/pixels.frag");
  loadModel(Model("models/sponza/sponza.obj"), this->_shader,
	    glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
	    glm::vec3(0.1f, 0.1f, 0.1f));
  loadVAO(
      shader_plane_vertices,
      loadShader(ShaderType::RAYMARCHING, "shaders/raymarching_mandelbox.frag"),
      {"", "", "", ""}, true, glm::vec3(-140.0f, 10.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::RAYMARCHING,
		     "shaders/raymarching_mandeltetra.frag"),
	  {"", "", "", ""}, true, glm::vec3(140.0f, 60.0f, 0.0f),
	  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::RAYMARCHING,
		     "shaders/raymarching_translucide.frag"),
	  {"", "", "", ""}, true, glm::vec3(0.0f, 58.0f, -50.0f),
	  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::RAYMARCHING,
		     "shaders/raymarching_mengersponge.frag"),
	  {"", "", "", ""}, true, glm::vec3(-140.0f, 60.0f, 0.0f),
	  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::NORMAL, "shaders/raymarching_terrain.frag"),
	  {"", "", "", ""}, false, glm::vec3(162.0f, 18.0f, 0.0f),
	  glm::vec3(0.0f, glm::radians(90.0f), 0.0f),
	  glm::vec3(22.0f, 24.0f, 22.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::NORMAL, "shaders/renderbuffer.frag"),
	  {"", "", "", ""}, false, glm::vec3(0.0f, 58.0f, 70.0f),
	  glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
	  glm::vec3(13.0f * _camera->getAspectRatio(), 13.0f, 15.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::RAYMARCHING,
		     "shaders/raymarching_volumetric_light.frag"),
	  {"", "", "", ""}, true, glm::vec3(0.0f, 10.0f, 50.0f),
	  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  loadVAO(shader_plane_vertices,
	  loadShader(ShaderType::RAYMARCHING,
		     "shaders/raymarching_volumetric_fog.frag"),
	  {"", "", "", ""}, true, glm::vec3(0.0f, 10.0f, -50.0f),
	  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
}

Scene::Scene(Scene const& src) { *this = src; }

Scene::~Scene(void) {
  for (VAO* vao : _scene_vaos) {
    delete vao;
  }
  for (Texture* tex : _scene_textures) {
    delete tex;
  }
  for (Shader* shader : _scene_shaders) {
    delete shader;
  }
  for (GameObject* entity : entities) {
    delete entity;
  }
  delete _camera;
}

Scene& Scene::operator=(Scene const& rhs) {
  if (this != &rhs) {
    this->_shader = rhs._shader;
    this->_camera = rhs._camera;
    this->_renderer = rhs._renderer;
    this->_paused = rhs._paused;
    this->_ended = rhs._ended;
    this->_scene_vaos = rhs._scene_vaos;
    this->_scene_textures = rhs._scene_textures;
  }
  return (*this);
}

void Scene::update(InputHandler& inputHandler, float deltaTime) {
  if (this->_ended) return;
  if (inputHandler.keys[GLFW_KEY_P]) {
    this->_paused = !this->_paused;
    inputHandler.keys[GLFW_KEY_P] = false;
  }
  if (this->_paused) return;
  cleanup();
  _camera->queryInput(inputHandler.keys, inputHandler.mousex,
		      inputHandler.mousey);
  _camera->update();
  for (auto& entity : this->entities) {
    entity->update(inputHandler, *this->_camera);
  }
  _renderer->uniforms.view = this->_camera->view;
  _renderer->uniforms.proj = this->_camera->proj;
  _renderer->uniforms.camPos = this->_camera->pos;
  _renderer->uniforms.camDir = this->_camera->dir;
  _renderer->uniforms.camRight = this->_camera->right;
  _renderer->uniforms.camUp = this->_camera->up;
  _renderer->uniforms.fovYscale = this->_camera->fovYscale;
  _renderer->uniforms.zNear = this->_camera->zNear;
  _renderer->uniforms.zFar = this->_camera->zFar;
  reloadShaders();
}

void Scene::draw() {
  for (const auto entity : this->entities) {
    this->_renderer->addRenderAttrib(entity->getRenderAttrib());
  }
  this->_renderer->draw();
  this->_renderer->flush();
}

void Scene::cleanup() {}

void Scene::loadModel(const Model& model, Shader* shader, glm::vec3 pos,
		      glm::vec3 rot, glm::vec3 scale) {
  for (const auto& mesh : model.meshes) {
    std::vector<Vertex> vertices;
    for (size_t i = mesh.vertexOffset;
	 i < model.vertices.size() && i < mesh.vertexOffset + mesh.indexCount;
	 i++) {
      vertices.push_back(model.vertices[i]);
    }
    loadVAO(vertices, shader,
	    {mesh.ambient_texname, mesh.diffuse_texname, mesh.specular_texname,
	     mesh.normal_texname},
	    false, pos, rot, scale);
  }
}

void Scene::loadVAO(std::vector<Vertex> vertices, Shader* shader,
		    std::array<std::string, 4> textures, bool billboard,
		    glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
  VAO* vao = new VAO(vertices);
  this->_scene_vaos.push_back(vao);
  GameObject* obj = new GameObject(shader, vao);
  obj->transform.scale = scale;
  obj->transform.position = pos;
  obj->transform.rotation = rot;
  obj->setChannel0(loadTexture(textures[0]));
  obj->setChannel1(loadTexture(textures[1]));
  obj->setChannel2(loadTexture(textures[2]));
  obj->setChannel3(loadTexture(textures[3]));
  for (Shader* dep : shader->dependencies) {
    if (dep->type == ShaderType::RENDERBUFFER) {
      if (dep->renderTarget->texture_channel == 0)
	obj->setChannel0(dep->renderTarget->texture);
      else if (dep->renderTarget->texture_channel == 1)
	obj->setChannel1(dep->renderTarget->texture);
      else if (dep->renderTarget->texture_channel == 2)
	obj->setChannel2(dep->renderTarget->texture);
      else if (dep->renderTarget->texture_channel == 3)
	obj->setChannel3(dep->renderTarget->texture);
    }
  }
  obj->setBillboard(billboard);
  this->entities.push_back(obj);
}

Texture* Scene::loadTexture(std::string filename) {
  if (filename.empty()) return (nullptr);
  for (Texture* tex : this->_scene_textures) {
    if (filename.compare(tex->filename) == 0) {
      return (tex);
    }
  }
  try {
    Texture* texture = new Texture(filename);
    this->_scene_textures.push_back(texture);
    return (texture);
  } catch (std::runtime_error) {
    return (nullptr);
  }
}
Shader* Scene::loadShader(ShaderType type, std::string frag_filename) {
  try {
    Shader* shader = new Shader(type, frag_filename);
    this->_scene_shaders.push_back(shader);
    return (shader);
  } catch (std::runtime_error) {
    return (nullptr);
  }
}

Shader* Scene::loadShader(ShaderType type, std::string vertex_filename,
			  std::string frag_filename) {
  try {
    Shader* shader = new Shader(type, vertex_filename, frag_filename);
    this->_scene_shaders.push_back(shader);
    return (shader);
  } catch (std::runtime_error) {
    return (nullptr);
  }
}

void Scene::reloadShaders() {
  for (Shader* shader : _scene_shaders) {
    shader->reload();
  }
}
