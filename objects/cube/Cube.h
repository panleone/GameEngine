#include "../../Camera.h"
#include "../Entity.h"
#include "../Light.h"

class Cube final : public Entity {
public:
  Cube(float size, Vec3f iPos = Vec3f(), Vec3f iVel = Vec3f(),
       Vec3f iAxis = Vec3f(), float iAngVel = 0.0f, float theta = 0.0f);
  void render(const Camera &camera) const override;
  void handleLights(std::span<Light *> lights) override;
};