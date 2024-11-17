#ifndef TEXTURE_C
#define TEXTURE_C

#include <string_view>

enum class TextureType { DIFFUSE, SPECULAR };

/**
 * This class represents a texture. It's neither movable nor copyable,
 * wrap it inside a shared_ptr to make multiple Models with the same texture
 */
class Texture {
private:
  unsigned int textureID;
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
  ~Texture();
};

#endif // TEXTURE_C