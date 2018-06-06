#include <deque>
#include <limits>
#include "camera.hpp"
#include "gameobject.hpp"
#include "model.hpp"
#include "pixels.hpp"
#include "renderer.hpp"
#include "shader.hpp"

class Scene {
 public:
  Scene(Renderer* renderer);
  Scene(Scene const& src);
  virtual ~Scene(void);
  Scene& operator=(Scene const& rhs);
  void update(InputHandler& inputHandler, float deltaTime);
  void draw();

 private:
  Scene(void);

  void cleanup();

  void loadModel(const Model& model, Shader* shader, glm::vec3 pos,
                 glm::vec3 rot, glm::vec3 scale);
  void loadVAO(std::vector<Vertex> vertices, Shader* shader,
               std::array<std::string, 4> textures, bool billboard,
               glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
  Texture* loadTexture(std::string filename);
  Shader* loadShader(ShaderType type, std::string vertex_filename,
                     std::string frag_filename);
  Shader* loadShader(ShaderType type, std::string frag_filename);
  void reloadShaders();

  Renderer* _renderer;
  Camera* _camera;
  Shader* _shader;
  bool _paused;
  bool _ended;

  std::vector<VAO*> _scene_vaos;
  std::vector<Texture*> _scene_textures;
  std::vector<Shader*> _scene_shaders;
  // std::vector<RenderTarget*> _render_target;
  std::vector<GameObject*> entities;
};
