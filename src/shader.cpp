#include "shader.hpp"

Shader::Shader(void) {}

Shader::Shader(Shader const &src) { *this = src; }

Shader::~Shader(void) {}

Shader &Shader::operator=(Shader const &rhs) {
  if (this != &rhs) {
    this->id = rhs.id;
    std::memcpy(this->_shaders, rhs._shaders, sizeof(this->_shaders));
  }
  return (*this);
}

Shader::Shader(std::string shader) : id(-1) {
  std::array<GLuint, 3> shader_ids;
  GLint err = -1;
  _shaders[0].filename = shader + shader_extensions[0];
  _shaders[1].filename = shader + shader_extensions[1];
  _shaders[2].filename = shader + shader_extensions[2];
  for (int i = 0; i < 3; i++) {
    shader_ids[i] = loadShader(_shaders[i].filename);
    _shaders[i].last_modification =
        getLastModificationTime(_shaders[i].filename);
  }

  id = linkShaders(shader_ids);
  glUseProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &err);
  if (GL_TRUE != err) {
    std::cerr << "Link error: (" << _shaders[0].filename << ", "
              << _shaders[1].filename << ", " << _shaders[2].filename << ")\n";
    printLinkError(id);
  }
}

GLuint Shader::loadShader(std::string &shader_filename) {
  int shader_type = 0;
  if (file_exists(shader_filename) == false) {
    shader_filename = "";
    return (0);
  }
  for (int i = 0; i < 3; i++) {
    if (shader_filename.find(shader_extensions[i]) != std::string::npos) {
      shader_type = shader_types[i];
    }
  }
  GLuint shader_id = compileShader(getShaderSource(shader_filename),
                                   shader_filename, shader_type);
  return (shader_id);
}

GLuint Shader::linkShaders(const std::array<GLuint, 3> shader_ids) {
  GLuint program_id = glCreateProgram();

  for (int i = 0; i < shader_ids.size(); i++) {
    if (shader_ids[i] != 0) {
      glAttachShader(program_id, shader_ids[i]);
    }
  }
  glLinkProgram(program_id);
  for (int i = 0; i < shader_ids.size(); i++) {
    if (shader_ids[i] != 0) {
      glDeleteShader(shader_ids[i]);
    }
  }
  return (program_id);
}

const std::string Shader::getShaderSource(std::string filename) {
  std::string line;
  std::string fileContent = "";
  int err = -1;
  if (filename.empty()) {
    return ("");
  }
  std::fstream shaderFile(filename);
  if (shaderFile) {
    while (getline(shaderFile, line)) fileContent += line + "\n";
    shaderFile.close();
  } else {
    std::cerr << "Invalid shader: " << filename << "\n";
  }
  return (fileContent);
}

GLuint Shader::compileShader(std::string source, std::string filename,
                             GLuint shaderType) {
  GLuint id = 0;
  int err = -1;

  if (shaderType != 0) {
    id = glCreateShader(shaderType);
    const char *fileContentChar = source.c_str();
    glShaderSource(id, 1, &fileContentChar, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &err);
    if (GL_TRUE != err) printShaderError(id, filename);
  }
  return (id);
}

void Shader::use() const { glUseProgram(this->id); }

void Shader::reload() {
  GLint err = -1;
  bool need_reload = false;
  for (int i = 0; i < 3; i++) {
    if (_shaders[i].filename.empty() == false &&
        _shaders[i].last_modification != -1) {
      std::time_t last_mod = getLastModificationTime(_shaders[i].filename);
      if (_shaders[i].last_modification < last_mod) {
        need_reload = true;
      }
    }
  }
  if (need_reload) {
    std::array<GLuint, 3> shader_ids;
    for (int i = 0; i < 3; i++) {
      shader_ids[i] = loadShader(_shaders[i].filename);
      _shaders[i].last_modification =
          getLastModificationTime(_shaders[i].filename);
    }

    id = linkShaders(shader_ids);
    glUseProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &err);
    if (GL_TRUE != err) {
      std::cerr << "Link error: (" << _shaders[0].filename << ", "
                << _shaders[1].filename << ", " << _shaders[2].filename
                << ")\n";
      printLinkError(id);
    }
  }
}

void printLinkError(GLuint program) {
  char log[2048];
  int max_length;
  int index;

  max_length = 2048;
  index = 0;
  glGetProgramInfoLog(program, max_length, &index, log);
  glGetShaderInfoLog(program, max_length, &index, log);
  std::cout << log << std::endl;
}

void printShaderError(GLuint shader, std::string file_name) {
  char log[2048];
  int max_length;
  int index;

  max_length = 2048;
  index = 0;
  glGetShaderInfoLog(shader, max_length, &index, log);
  std::cerr << "Cannot compile shader: " << file_name << "\n" << log;
}

bool file_exists(std::string filename) {
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (true);
  }
  return (false);
}

std::time_t getLastModificationTime(std::string filename) {
  struct stat result;
  if (!filename.empty()) {
    if (stat(filename.c_str(), &result) == 0) {
      std::time_t mod_time = result.st_mtime;
      return (mod_time);
    }
  }
  return ((std::time_t)-1);
}
