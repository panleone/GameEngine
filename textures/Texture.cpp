#include "Texture.h"

#include <format>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int getImageFormat(unsigned int nChannels) {
  switch (nChannels) {
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    throw std::runtime_error(
        std::format("Number of channels not supported {}!", nChannels));
  }
}
Texture::Texture(std::string_view texturePath, TextureType type, bool gammaCorr)
    : type{type} {
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(texturePath.data(), &width, &height, &nChannels, 0);
  if (!data) {
    throw std::runtime_error(
        std::format("Cannot load texture {}!", texturePath));
  }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  unsigned int outputFormat = gammaCorr ? GL_SRGB : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, outputFormat, width, height, 0,
               getImageFormat(nChannels), GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, textureID); }

Texture::~Texture() { glDeleteBuffers(1, &textureID); }