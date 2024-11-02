#ifndef MODEL_C
#define MODEL_C

#include <functional>
#include <span>
#include <string_view>

#include "../textures/Texture.h"

class Model {
private:
  unsigned int vboID;
  unsigned int vaoID;
  unsigned int eboID;
  size_t nVertices;
  // Vector of pairs <GL_TEXTURE, Texture>
  std::vector<std::pair<unsigned int, Texture>> textures;

public:
  Model(std::span<float> vertices, std::span<unsigned int> indices,
        std::function<void()> attrPointers);
  ~Model();
  void bindAndDraw() const;
  void addTexture(unsigned int textureSlot, std::string_view texturePath);
};

#endif // MODEL_C