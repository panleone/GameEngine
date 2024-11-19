#ifndef LIGHT_C
#define LIGHT_C

#include "../math/Matrix.h"
#include "Entity.h"

class Light : public Entity {
public:
  Vec3f lightColor;
  Vec3f ambientIntensity = Vec3f();
  Vec3f diffuseIntensity = Vec3f();
  Vec3f specularIntensity = Vec3f();
  // By default set a realistic 1/d^2 attenuation
  Vec3f attenuationCoefficients = Vec3f{0.0f, 0.0f, 1.0f};

public:
  Light(Model model, Vec3f lightColor)
      : Entity{std::move(model)}, lightColor{std::move(lightColor)} {
    // Default values that seems to give good results
    ambientIntensity = this->lightColor * 0.1f;
    diffuseIntensity = this->lightColor * 0.25f;
    specularIntensity = this->lightColor.clone();
  };
};

#endif // LIGHT_C