#ifndef TEXTURE_C
#define TEXTURE_C

#include <string_view>

#include "../buffer/Buffer.h"

enum class TextureType { DIFFUSE, SPECULAR };

/**
 * RAII class to take in account the throwing destructor of Texture
 */
class stbiWrapper {
public:
  int width;
  int height;
  int nChannels;
  stbiWrapper(std::string_view texturePath);
  ~stbiWrapper();
  stbiWrapper(const stbiWrapper &wrapper) = delete;
  stbiWrapper(stbiWrapper &&wrapper) = delete;
  const unsigned char *getData() const { return data; };

private:
  unsigned char *data{nullptr};
};

/**
 * This class represents a texture. It's neither movable nor copyable,
 * wrap it inside a shared_ptr to make multiple Models with the same texture
 */
class Texture {
private:
  Buffer<BUFFER_TYPE::TEXTURE> rawTexture;
  TextureType type;

public:
  /**
   * Create a texture from a file
   * @param texturePath - path to the texture file (e.g ./.../*.png)
   * @param type - type of the texture
   * @param gammaCorr - whether to enable gamma correction, true by default
   */
  Texture(std::string_view texturePath, TextureType type,
          bool gammaCorr = true);
  Texture(const Texture &texture) = delete;
  Texture(Texture &&texture) = delete;
  void bind() const;
  TextureType getType() const { return type; };
};

#endif // TEXTURE_C