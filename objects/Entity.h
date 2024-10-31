#ifndef ENTITY_C
#define ENTITY_C

#include <memory>

#include "../Camera.h"
#include "../math/Matrix.h"
#include "Light.h"
#include "Model.h"
#include "../shaders/Shader.h"

class Entity {
protected:
  std::unique_ptr<Model> model;
  std::unique_ptr<ShaderProgram> program;

public:
  // Linear coordinates
  Vec3f position;
  Vec3f velocity;
  // Angular coordinates
  float theta;
  Vec3f rotationAxis;
  float angularVelocity;
  // Scaling
  float scale = 1.0f;

  Entity(Vec3f iPos = Vec3f(), Vec3f iVel = Vec3f(), Vec3f iAxis = Vec3f(),
         float iAngVel = 0.0f, float theta = 0.0f)
      : position{std::move(iPos)}, velocity{std::move(iVel)},
        rotationAxis{std::move(iAxis)}, angularVelocity{iAngVel},
        theta{theta} {};
  void update(float deltaT) {
    position += velocity * deltaT;
    theta += angularVelocity * deltaT;
  }
  virtual void render(const Camera &camera) const = 0;
  virtual void updateLight(const Light &light) = 0;
  virtual ~Entity() = default;

  Mat4f modelMatrix() const {
    return mat::translate(position) * mat::scale(Vec3f{scale, scale, scale}) *
           mat::rotate(theta, rotationAxis);
  }
};
#endif // ENTITY_C