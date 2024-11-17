#include "Texture.h"

#include <format>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int getImageFormat(std::string_view texturePath) {
  if (texturePath.ends_with(".jpg")) {
    return GL_RGB;
  } else if (texturePath.ends_with(".png")) {
    return GL_RGBA;
  } else {
    throw std::runtime_error(
        std::format("Texture format {} not supported!", texturePath));
  }
}
Texture::Texture(std::string_view texturePath, TextureType type) : type{type} {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);
  if (!data) {
    throw std::runtime_error(
        std::format("Cannot load texture {}!", texturePath));
  }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               getImageFormat(texturePath), GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, textureID); }

Texture::~Texture() { glDeleteBuffers(1, &textureID); }