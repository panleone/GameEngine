#include "Texture.h"

#include <format>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int getImageInputFormat(unsigned int nChannels) {
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
static unsigned int getImageOutputFormat(unsigned int nChannels,
                                         bool gammaCorr) {
  if (!gammaCorr) {
    return getImageInputFormat(nChannels);
  }
  switch (nChannels) {
  case 3:
    return GL_SRGB;
  case 4:
    return GL_SRGB_ALPHA;
  default:
    throw std::runtime_error(
        std::format("Number of channels not supported {}!", nChannels));
  }
}

Texture::Texture(std::string_view texturePath, TextureType type, bool gammaCorr)
    : type{type} {
  stbi_set_flip_vertically_on_load(true);
  stbiWrapper wrapper{texturePath};
  rawTexture.bind();
  unsigned int inputFormat = getImageInputFormat(wrapper.nChannels);
  unsigned int outputFormat =
      getImageOutputFormat(wrapper.nChannels, gammaCorr);
  glTexImage2D(GL_TEXTURE_2D, 0, outputFormat, wrapper.width, wrapper.height, 0,
               inputFormat, GL_UNSIGNED_BYTE, wrapper.getData());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind() const { rawTexture.bind(); }

stbiWrapper::stbiWrapper(std::string_view texturePath) {
  data = stbi_load(texturePath.data(), &width, &height, &nChannels, 0);
  if (!data) {
    // Ok in this case destructor must not be called anyways
    throw std::runtime_error(
        std::format("Cannot load texture {}!", texturePath));
  }
}
stbiWrapper::~stbiWrapper() { stbi_image_free(data); }