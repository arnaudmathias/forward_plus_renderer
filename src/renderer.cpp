#include "renderer.hpp"

namespace render {

Renderer::Renderer(int width, int height) : _width(width), _height(height) {
  switchDepthTestState(true);
  switchBlendingState(true);
  switchDepthTestFunc(DepthTestFunc::Less);
  switchBlendingFunc(BlendFunc::OneMinusSrcAlpha);

  // Gen and setup our depth depth map FBO for the depth prepass
  glGenFramebuffers(1, &depthpass_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, depthpass_fbo);

  glGenTextures(1, &depthpass_texture_depth_id);
  glBindTexture(GL_TEXTURE_2D, depthpass_texture_depth_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthpass_texture_depth_id, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Could not validate framebuffer" << std::endl;
  }

  glGenFramebuffers(1, &lightpass_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, lightpass_fbo);

  // HDR buffer
  glGenTextures(1, &lightpass_texture_hdr_id);
  glBindTexture(GL_TEXTURE_2D, lightpass_texture_hdr_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         lightpass_texture_hdr_id, 0);

  // Normal buffer
  glGenTextures(1, &lightpass_texture_normal_id);
  glBindTexture(GL_TEXTURE_2D, lightpass_texture_normal_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         lightpass_texture_normal_id, 0);

  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);

  glGenTextures(1, &lightpass_texture_depth_id);
  glBindTexture(GL_TEXTURE_2D, lightpass_texture_depth_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         lightpass_texture_depth_id, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Could not validate framebuffer" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if (GLVersion.major >= 4 && GLVersion.minor >= 3) {
    // Lights SSBO
    glGenBuffers(1, &ssbo_lights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_lights);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Lights), &uniforms.lights,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_lights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLuint workgroup_x = (_width + (_width % TILE_SIZE)) / TILE_SIZE;
    GLuint workgroup_y = (_height + (_height % TILE_SIZE)) / TILE_SIZE;
    // Visible light indices SSBO
    glGenBuffers(1, &ssbo_visible_lights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_visible_lights);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(int) * workgroup_x * workgroup_y * MAX_LIGHTS_PER_TILE,
                 NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_visible_lights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  } else {
    glGenBuffers(1, &ssbo_lights);
    glBindBuffer(GL_UNIFORM_BUFFER, ssbo_lights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), &uniforms.lights,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ssbo_lights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  // Material UBO
  glGenBuffers(1, &ubo_id);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UBO), &ubo, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_id);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  std::vector<glm::vec4> vertices_quad = {
      {-1.0f, 1.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, 0.0f, 1.0f},
      {1.0f, -1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 0.0f, 0.0f},
      {1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f}};
  std::vector<glm::vec3> vertices_octa = {
      {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}};
  std::vector<unsigned int> indices_octa = {0, 2, 3, 4, 2, 0, 0, 5, 4, 3, 5, 0,
                                            1, 2, 4, 3, 2, 1, 1, 5, 3, 4, 5, 1};
  _vao_quad = std::make_shared<VAO>(vertices_quad);
  _vao_octahedron = std::make_shared<VAO>(vertices_octa, indices_octa);
}

Renderer::Renderer(Renderer const &src) { *this = src; }

Renderer::~Renderer(void) {
  glDeleteBuffers(1, &ssbo_lights);
  glDeleteBuffers(1, &ssbo_visible_lights);
  glDeleteBuffers(1, &ubo_id);

  glDeleteTextures(1, &depthpass_texture_depth_id);
  glDeleteFramebuffers(1, &depthpass_fbo);

  glDeleteTextures(1, &lightpass_texture_hdr_id);
  glDeleteTextures(1, &lightpass_texture_normal_id);
  glDeleteTextures(1, &lightpass_texture_depth_id);
  glDeleteFramebuffers(1, &lightpass_fbo);
}

Renderer &Renderer::operator=(Renderer const &rhs) {
  if (this != &rhs) {
    this->_attribs = rhs._attribs;
  }
  return (*this);
}

void Renderer::renderText(float pos_x, float pos_y, float scale,
                          std::string text, glm::vec3 color) {
  RenderState backup_state = _state;

  switchPolygonMode(PolygonMode::Fill);
  switchDepthTestState(true);
  switchBlendingFunc(BlendFunc::OneMinusSrcAlpha);
  switchBlendingState(true);

  TextProperties prop = {};
  prop.color = color;
  _textRenderer.renderText(_shaderCache.getShader("text"), pos_x, pos_y, scale,
                           text, prop);

  setState(backup_state);
}

void Renderer::renderUI(std::string filename, float pos_x, float pos_y,
                        float scale, bool centered) {
  RenderState backup_state = _state;

  switchPolygonMode(PolygonMode::Fill);
  switchDepthTestState(false);
  switchBlendingFunc(BlendFunc::OneMinusSrcAlpha);
  switchBlendingState(true);

  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->_width),
                                    0.0f, static_cast<float>(this->_height));
  _uiRenderer.renderUI(_shaderCache.getShader("ui"), filename, pos_x, pos_y,
                       scale, projection, centered);

  setState(backup_state);
}
void Renderer::bindTexture(GLuint texture_id, GLenum tex_slot) {
  if (texture_id != 0) {
    glActiveTexture(tex_slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
  } else {
    glActiveTexture(tex_slot);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Renderer::addAttrib(const Attrib &attrib) {
  this->_attribs.push_back(attrib);
}

void Renderer::switchShader(GLuint shader_id, int &current_shader_id) {
  if (shader_id > 0 && shader_id != current_shader_id) {
    glUseProgram(shader_id);
    setUniform(glGetUniformLocation(shader_id, "P"), uniforms.proj);
    setUniform(glGetUniformLocation(shader_id, "invP"), uniforms.inv_proj);
    setUniform(glGetUniformLocation(shader_id, "V"), uniforms.view);
    setUniform(glGetUniformLocation(shader_id, "VP"), uniforms.view_proj);
    setUniform(glGetUniformLocation(shader_id, "view_pos"), uniforms.view_pos);
    setUniform(glGetUniformLocation(shader_id, "num_lights"), NUM_LIGHTS);
    setUniform(glGetUniformLocation(shader_id, "screen_size"),
               uniforms.screen_size);
    current_shader_id = shader_id;
  }
}

void Renderer::updateUniforms(const Attrib &attrib, const int shader_id) {
  if (shader_id > 0) {
    glm::mat4 mvp = uniforms.view_proj * attrib.model;
    setUniform(glGetUniformLocation(shader_id, "MVP"), mvp);
    setUniform(glGetUniformLocation(shader_id, "MV"),
               uniforms.view * attrib.model);
    setUniform(glGetUniformLocation(shader_id, "M"), attrib.model);
    setUniform(glGetUniformLocation(shader_id, "albedo_tex"), 0);
    setUniform(glGetUniformLocation(shader_id, "metallic_tex"), 1);
    setUniform(glGetUniformLocation(shader_id, "roughness_tex"), 2);
    setUniform(glGetUniformLocation(shader_id, "normal_tex"), 3);
  }
}

void Renderer::draw() {
  RenderState backup_state = _state;
  int current_shader_id = -1;

  std::shared_ptr<Shader> depthprepass = _shaderCache.getShader("depthprepass");
  std::shared_ptr<Shader> lightculling = _shaderCache.getShader("lightculling");
  std::shared_ptr<Shader> shading = _shaderCache.getShader("shading");
  std::shared_ptr<Shader> octahedron = _shaderCache.getShader("octahedron");
  std::shared_ptr<Shader> def = _shaderCache.getShader("default");

  glViewport(0, 0, _width, _height);
  // Depth prepass
  // Bind the framebuffer and render opaque scene geometry
  {
    glBindFramebuffer(GL_FRAMEBUFFER, depthpass_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    switchDepthTestState(true);
    switchDepthTestFunc(DepthTestFunc::Less);

    switchShader(depthprepass->id, current_shader_id);
    for (const auto &attrib : this->_attribs) {
      if (attrib.alpha_mask == false) {
        updateUniforms(attrib, depthprepass->id);
        drawVAOs(attrib.vao, attrib.state.primitiveMode);
      }
    }
  }

  // Copy the depth buffer to the light pass framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, depthpass_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightpass_fbo);
  glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height,
                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  GLuint workgroup_x = (_width + (_width % TILE_SIZE)) / TILE_SIZE;
  GLuint workgroup_y = (_height + (_height % TILE_SIZE)) / TILE_SIZE;
  // Update Lights buffer
  if (GLVersion.major >= 4 && GLVersion.minor >= 3) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_lights);
    GLvoid *lights_ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(lights_ptr, &uniforms.lights, sizeof(Lights));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  } else {
    glBindBuffer(GL_UNIFORM_BUFFER, ssbo_lights);
    GLvoid *lights_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(lights_ptr, &uniforms.lights, sizeof(Lights));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }
  if (GLVersion.major >= 4 && GLVersion.minor >= 3) {
    // Light culling
    {
      glBindFramebuffer(GL_FRAMEBUFFER, lightpass_fbo);
      switchDepthTestFunc(DepthTestFunc::Equal);
      glClear(GL_COLOR_BUFFER_BIT);

      switchShader(lightculling->id, current_shader_id);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_lights);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_visible_lights);

      glActiveTexture(GL_TEXTURE0);
      setUniform(glGetUniformLocation(lightculling->id, "depthmap"), 0);
      glBindTexture(GL_TEXTURE_2D, depthpass_texture_depth_id);

      glDispatchCompute(workgroup_x, workgroup_y, 1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
  }

  // Light pass
  {
    glBindFramebuffer(GL_FRAMEBUFFER, lightpass_fbo);
    switchDepthTestFunc(DepthTestFunc::Equal);
    glClear(GL_COLOR_BUFFER_BIT);

    switchShader(shading->id, current_shader_id);
    setUniform(glGetUniformLocation(shading->id, "workgroup_x"),
               static_cast<int>(workgroup_x));
    setUniform(glGetUniformLocation(shading->id, "debug"),
               uniforms.visibilty_debug);

    if (GLVersion.major >= 4 && GLVersion.minor >= 3) {
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_lights);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_visible_lights);
      glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_id);
    } else {
      glBindBufferBase(GL_UNIFORM_BUFFER, 0, ssbo_lights);
      glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_id);
    }

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, uniforms.albedo_array->id);
    setUniform(glGetUniformLocation(shading->id, "albedo_array"), 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, uniforms.normal_array->id);
    setUniform(glGetUniformLocation(shading->id, "normal_array"), 1);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, uniforms.metallic_array->id);
    setUniform(glGetUniformLocation(shading->id, "metallic_array"), 2);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, uniforms.roughness_array->id);
    setUniform(glGetUniformLocation(shading->id, "roughness_array"), 3);

    switchBlendingState(false);
    for (const auto &attrib : this->_attribs) {
      if (attrib.alpha_mask == false) {
        updateUniforms(attrib, shading->id);
        setUniform(glGetUniformLocation(shading->id, "albedo_tex"),
                   attrib.albedo_index);
        setUniform(glGetUniformLocation(shading->id, "metallic_tex"),
                   attrib.metallic_index);
        setUniform(glGetUniformLocation(shading->id, "roughness_tex"),
                   attrib.roughness_index);
        setUniform(glGetUniformLocation(shading->id, "normal_tex"),
                   attrib.normal_index);
        drawVAOs(attrib.vao, attrib.state.primitiveMode);
      }
    }
    switchBlendingState(true);
    switchDepthTestFunc(DepthTestFunc::Less);
    for (const auto &attrib : this->_attribs) {
      if (attrib.alpha_mask == true) {
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_id);
        updateUniforms(attrib, shading->id);
        setUniform(glGetUniformLocation(shading->id, "albedo_tex"),
                   attrib.albedo_index);
        setUniform(glGetUniformLocation(shading->id, "metallic_tex"),
                   attrib.metallic_index);
        setUniform(glGetUniformLocation(shading->id, "roughness_tex"),
                   attrib.roughness_index);
        setUniform(glGetUniformLocation(shading->id, "normal_tex"),
                   attrib.normal_index);
        drawVAOs(attrib.vao, attrib.state.primitiveMode);
      }
    }
    if (uniforms.light_debug) {
      switchDepthTestFunc(DepthTestFunc::Less);
      switchShader(octahedron->id, current_shader_id);
      for (unsigned int i = 0; i < NUM_LIGHTS; ++i) {
        Light light = uniforms.lights.lights[i];
        setUniform(glGetUniformLocation(octahedron->id, "color"), light.color);
        glm::mat4 model =
            glm::translate(light.position) * glm::scale(glm::vec3(0.15f));
        glm::mat4 mvp = uniforms.view_proj * model;
        setUniform(glGetUniformLocation(octahedron->id, "MVP"), mvp);
        glBindVertexArray(_vao_octahedron->vao);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, _vao_octahedron->indices_size,
                       GL_UNSIGNED_INT, 0);
      }
    }
  }

  // Assembly
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    switchDepthTestState(false);
    glClear(GL_COLOR_BUFFER_BIT);
    switchShader(def->id, current_shader_id);
    setUniform(glGetUniformLocation(def->id, "hdr_tex"), 5);

    bindTexture(lightpass_texture_hdr_id, GL_TEXTURE0 + 5);

    glBindVertexArray(_vao_quad->vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  setState(backup_state);

  glBindVertexArray(0);
}

void Renderer::drawVAOs(std::shared_ptr<VAO> vao,
                        render::PrimitiveMode primitive_mode) {
  GLenum mode = getGLRenderMode(primitive_mode);
  if (vao != nullptr) {
    if (vao->indices_size != 0) {
      glBindVertexArray(vao->vao);
      glDrawElements(mode, vao->indices_size, GL_UNSIGNED_INT, 0);
    } else if (vao->vertices_size != 0) {
      glBindVertexArray(vao->vao);
      glDrawArrays(mode, 0, vao->vertices_size);
    }
  }
}

void Renderer::update(const Env &env) {
  if (env.width != _width || env.height != _height) {
    _width = env.width;
    _height = env.height;
    updateRessources();
  }
  _textRenderer.update(env);
  _shaderCache.update();
}

void Renderer::updateRessources() {
  GLuint workgroup_x = (_width + (_width % TILE_SIZE)) / TILE_SIZE;
  GLuint workgroup_y = (_height + (_height % TILE_SIZE)) / TILE_SIZE;
  if (GLVersion.major >= 4 && GLVersion.minor >= 3) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_visible_lights);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(int) * workgroup_x * workgroup_y * MAX_LIGHTS_PER_TILE,
                 NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  } else {
    glBindBuffer(GL_UNIFORM_BUFFER, ssbo_visible_lights);
    glBufferData(GL_UNIFORM_BUFFER,
                 sizeof(int) * workgroup_x * workgroup_y * MAX_LIGHTS_PER_TILE,
                 NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  glBindTexture(GL_TEXTURE_2D, depthpass_texture_depth_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glBindTexture(GL_TEXTURE_2D, lightpass_texture_hdr_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA,
               GL_FLOAT, NULL);

  glBindTexture(GL_TEXTURE_2D, lightpass_texture_normal_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB,
               GL_FLOAT, NULL);

  glBindTexture(GL_TEXTURE_2D, lightpass_texture_depth_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void Renderer::flushAttribs() { this->_attribs.clear(); }

int Renderer::getScreenWidth() { return (this->_width); }

int Renderer::getScreenHeight() { return (this->_height); }

void Renderer::clearScreen() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLenum Renderer::getGLRenderMode(PrimitiveMode mode) {
  GLenum gl_primitive_modes[12] = {GL_POINTS,
                                   GL_LINE_STRIP,
                                   GL_LINE_LOOP,
                                   GL_LINES,
                                   GL_LINE_STRIP_ADJACENCY,
                                   GL_LINES_ADJACENCY,
                                   GL_TRIANGLE_STRIP,
                                   GL_TRIANGLE_FAN,
                                   GL_TRIANGLES,
                                   GL_TRIANGLE_STRIP_ADJACENCY,
                                   GL_TRIANGLES_ADJACENCY,
                                   GL_PATCHES};
  unsigned index_mode = static_cast<unsigned int>(mode);
  return (gl_primitive_modes[index_mode]);
}

void Renderer::setState(const RenderState &new_state) {
  switchPolygonMode(new_state.polygonMode);
  switchDepthTestFunc(new_state.depthTestFunc);
  switchDepthTestState(new_state.depthTest);
  switchBlendingState(new_state.blending);
  switchBlendingFunc(new_state.blendFunc);
}

void Renderer::switchPolygonMode(PolygonMode mode) {
  GLenum gl_polygon_modes[3] = {GL_POINT, GL_LINE, GL_FILL};
  if (mode != _state.polygonMode) {
    unsigned int index_mode = static_cast<unsigned int>(mode);
    glPolygonMode(GL_FRONT_AND_BACK, gl_polygon_modes[index_mode]);
    _state.polygonMode = mode;
  }
}

void Renderer::switchDepthTestFunc(DepthTestFunc mode) {
  GLenum gl_depth_funcs[8] = {GL_LESS,    GL_NEVER,    GL_EQUAL,  GL_LEQUAL,
                              GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS};
  if (mode != _state.depthTestFunc) {
    unsigned int index_func = static_cast<unsigned int>(mode);
    glDepthFunc(gl_depth_funcs[index_func]);
    _state.depthTestFunc = mode;
  }
}

void Renderer::switchBlendingFunc(BlendFunc mode) {
  GLenum gl_blend_funcs[14] = {GL_ZERO,           GL_ONE,
                               GL_SRC_COLOR,      GL_ONE_MINUS_SRC_COLOR,
                               GL_DST_COLOR,      GL_ONE_MINUS_DST_COLOR,
                               GL_SRC_ALPHA,      GL_ONE_MINUS_SRC_ALPHA,
                               GL_DST_ALPHA,      GL_ONE_MINUS_DST_ALPHA,
                               GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
                               GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA};
  if (mode != _state.blendFunc) {
    unsigned int index_func = static_cast<unsigned int>(mode);
    glBlendFunc(GL_SRC_ALPHA, gl_blend_funcs[index_func]);
    _state.blendFunc = mode;
  }
}

void Renderer::switchDepthTestState(bool depth_test) {
  if (depth_test != _state.depthTest) {
    if (depth_test) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
    _state.depthTest = depth_test;
  }
}

void Renderer::switchBlendingState(bool blending) {
  if (blending != _state.blending) {
    if (blending) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
    _state.blending = blending;
  }
}

bool Attrib::operator<(const struct Attrib &rhs) const { return (true); }

}  // namespace render
