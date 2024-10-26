#ifndef ENTITY_C
#define ENTITY_C

#include <memory>

#include "../math/Matrix.h"
#include "Model.h"
#include "Shader.h"

class Entity {
protected:
  std::unique_ptr<Model> model;
  std::unique_ptr<ShaderProgram> program;

public:
  Vec3f position;
  Vec3f velocity;
  Entity(Vec3f iPos = {0.0f, 0.0f, 0.0f}, Vec3f iVel = {0.0f, 0.0f, 0.0f})
      : position{std::move(iPos)}, velocity{std::move(iVel)} {}
  void update(float deltaT) { position += velocity * deltaT; }
  virtual void render() const = 0;

  virtual ~Entity() = default;
};
#endif // ENTITY_C