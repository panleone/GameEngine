#ifndef ENTITY_C
#define ENTITY_C

#include "../math/Matrix.h"
#include "../math/MatrixUtils.h"
#include "../shaders/Shader.h"
#include "Model.h"

class Entity {
private:
  Model model;
  // Scaling
  Vec4f scale{1.0f, 1.0f, 1.0f, 1.0f};

public:
  // Linear coordinates
  Vec3f position = Vec3f();
  Vec3f velocity = Vec3f();
  // Angular coordinates
  float theta = 0.0f;
  Vec3f rotationAxis = Vec3f();
  float angularVelocity = 0.0f;

  Entity(Model model) : model{std::move(model)} {};
  void update(float deltaT) {
    position += velocity * deltaT;
    theta += angularVelocity * deltaT;
  }
  void render(const ShaderProgram &program) const { model.render(program); };
  void setScale(float newScale) {
    scale = Vec4f{newScale, newScale, newScale, 1.0f};
  };
  Mat4f modelMatrix() const {
    Mat4f translation = mat::translate(position);
    mat::multByDiagonal(translation, scale);
    return translation * mat::rotate(theta, rotationAxis);
  }
};
#endif // ENTITY_C