#ifndef MODEL_C
#define MODEL_C

#include <functional>
#include <span>

class Model {
private:
  unsigned int vboID;
  unsigned int vaoID;
  unsigned int eboID;
  size_t nVertices;

public:
  Model(std::span<float> vertices, std::span<unsigned int> indices,
        std::function<void()> attrPointers);
  ~Model();
  void bindAndDraw() const;
};

#endif // MODEL_C