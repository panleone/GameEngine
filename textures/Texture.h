#ifndef TEXTURE_C
#define TEXTURE_C

#include <string_view>

class Texture {
  unsigned int textureId;

public:
  Texture(std::string_view texturePath);
  Texture(const Texture &texture) = delete;
  Texture(Texture &&texture) = default;
  void bind(unsigned int textureSlot) const;
  ~Texture();
};

#endif // TEXTURE_C