#ifndef LIGHT_C
#define LIGHT_C

#include "../math/Matrix.h"

class Light : public Entity {
public:
  Vec3f lightColor;
  Vec3f ambientIntensity = Vec3f();
  Vec3f diffuseIntensity = Vec3f();
  Vec3f specularIntensity = Vec3f();
  Vec3f attenuationCoefficients = Vec3f();

public:
  Light(Model model, Vec3f lightColor)
      : Entity{std::move(model)}, lightColor{std::move(lightColor)} {};
};

#endif // LIGHT_C