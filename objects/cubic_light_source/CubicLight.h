#ifndef CUBICLIGHT_C
#define CUBICLIGHT_C

#include "../../Camera.h"
#include "../Entity.h"
#include "../Light.h"

class CubicLight final : public Entity, public Light {
private:
  Vec3f lightColor = Vec3f{1.0f, 1.0f, 1.0f};

public:
  CubicLight(float size, Vec3f iPos = Vec3f(), Vec3f iVel = Vec3f(),
             Vec3f iAxis = Vec3f(), float iAngVel = 0.0f, float theta = 0.0f);

  void render(const Camera &camera) const override;

  Vec3f getPosition() const override;
  Vec3f ambientIntensity() const override;
  Vec3f diffuseIntensity() const override;
  Vec3f specularIntensity() const override;

  // For the moment light sources are not affected by other light sources
  void updateLight(const Light &light) override { return; };
};

#endif // CUBICLIGHT_C