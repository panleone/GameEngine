#ifndef ENTITY_C
#define ENTITY_C

#include "../math/Matrix.h"
#include "../math/MatrixUtils.h"
#include "../shaders/Shader.h"
#include "Model.h"

class Entity {
private:
  Model model;

public:
  // Linear coordinates
  Vec3f position = Vec3f();
  Vec3f velocity = Vec3f();
  // Angular coordinates
  float theta = 0.0f;
  Vec3f rotationAxis = Vec3f();
  float angularVelocity = 0.0f;
  // Scaling
  float scale = 1.0f;

  Entity(Model model) : model{std::move(model)} {};
  void update(float deltaT) {
    position += velocity * deltaT;
    theta += angularVelocity * deltaT;
  }
  void render(const ShaderProgram &program) const { model.render(program); };

  Mat4f modelMatrix() const {
    return mat::translate(position) * mat::scale(Vec3f{scale, scale, scale}) *
           mat::rotate(theta, rotationAxis);
  }
};
#endif // ENTITY_C