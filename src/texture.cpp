#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(std::string filename) : id(0), filename(filename) {
  int texChannels;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc* pixels = stbi_load(filename.c_str(), &this->width, &this->height,
                              &texChannels, STBI_rgb_alpha);
  if (pixels != nullptr) {
    glGenTextures(1, &this->id);

    glBindTexture(GL_TEXTURE_2D, this->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

Texture::Texture(std::string filename, int offset_x, int offset_y) {
  int texChannels;
  // stbi_set_flip_vertically_on_load(true);
  stbi_uc* pixels = stbi_load(filename.c_str(), &this->width, &this->height,
                              &texChannels, STBI_rgb_alpha);
  if (pixels != nullptr) {
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);
    int depth = (this->width / offset_x) * (this->height / offset_y);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, offset_x, offset_y, depth, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    int zoffset = 0;
    unsigned char* sub_pixels =
        new unsigned char[offset_x * offset_y * texChannels];
    // Parse texture atlas and generate sub textures at each offset
    for (int y_atlas = 0; y_atlas < height; y_atlas += offset_y) {
      for (int x_atlas = 0; x_atlas < width; x_atlas += offset_x) {
        int target_offset = 0;
        for (int y_local = y_atlas; y_local < y_atlas + offset_y; y_local++) {
          for (int x_local = x_atlas; x_local < x_atlas + offset_x; x_local++) {
            int source_offset =
                (y_local * (width * texChannels) + (x_local * texChannels));
            std::memcpy(sub_pixels + target_offset, pixels + source_offset,
                        texChannels);
            target_offset += texChannels;
          }
        }
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, zoffset, offset_x,
                        offset_y, 1, GL_RGBA, GL_UNSIGNED_BYTE, sub_pixels);
        zoffset++;
      }
    }
    delete[] sub_pixels;
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    throw std::runtime_error("Cannot load texture (" + filename + ")");
  }
}

Texture::Texture(const std::vector<std::string>& textures) : id(0) {
  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
  int width, height, nrChannels;
  for (unsigned int i = 0; i < textures.size(); i++) {
    unsigned char* data =
        stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      stbi_image_free(data);
      throw std::runtime_error("Cannot load cube map texture (" + textures[i] +
                               ")");
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

Texture::~Texture() {
  if (this->id != 0) {
    glDeleteTextures(1, &this->id);
  }
}

TextureArray::TextureArray(const std::vector<std::string>& textures) {
  std::set<std::string> texture_set;

  for (const auto& texture : textures) {
    texture_set.insert(texture);
  }
  GLint internal_format = 0;
  GLenum format = 0;
  stbi_set_flip_vertically_on_load(true);
  int zoffset = 0;
  for (auto it : texture_set) {
    int tex_channels;
    stbi_uc* pixels =
        stbi_load(it.c_str(), &width, &height, &tex_channels, STBI_default);

    if (pixels != nullptr) {
      if (id == 0) {
        if (tex_channels == 1) {
          internal_format = GL_R8;
          format = GL_RED;
        } else if (tex_channels == 4) {
          internal_format = GL_RGBA8;
          format = GL_RGBA;
        } else {
          assert(0);
        }
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal_format, width, height,
                     static_cast<GLuint>(texture_set.size()), 0, format,
                     GL_UNSIGNED_BYTE, NULL);
      }
      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, zoffset, width, height, 1,
                      format, GL_UNSIGNED_BYTE, pixels);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                      GL_NEAREST_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

      stbi_image_free(pixels);
      _lookup_table.emplace(it, zoffset);
      zoffset++;
    }
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

int TextureArray::getTextureIndex(std::string texture_name) {
  auto res = _lookup_table.find(texture_name);
  if (res != _lookup_table.end()) {
    return (res->second);
  }
  std::cout << "unable to find: " << texture_name << std::endl;
  return (-1);
}

TextureArray::~TextureArray() {
  if (this->id != 0) {
    glDeleteTextures(1, &this->id);
  }
}
