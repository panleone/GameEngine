#ifndef LIGHT_C
#define LIGHT_C

#include "../math/Matrix.h"
#include "../shaders/Shader.h"
#include "Entity.h"

enum class LightType { POINT_LIGHT, DIRECTIONAL };

class Light {
public:
  Vec3f lightColor;
  Vec3f ambientIntensity;
  Vec3f diffuseIntensity;
  Vec3f specularIntensity;
  Light(Vec3f lightColor) : lightColor{std::move(lightColor)} {
    // Default values that seems to give good results
    ambientIntensity = this->lightColor * 0.1f;
    diffuseIntensity = this->lightColor * 0.25f;
    specularIntensity = this->lightColor.clone();
  }
};

class PointLight : public Entity, public Light {
public:
  // By default set a realistic 1/d^2 attenuation
  Vec3f attenuationCoefficients = Vec3f{0.0f, 0.0f, 1.0f};

  PointLight(Model model, Vec3f lightColor)
      : Entity{std::move(model)}, Light{std::move(lightColor)} {};
};

class DirectionalLight : public Light {
public:
  Vec3f direction;
  DirectionalLight(Vec3f direction, Vec3f lightColor)
      : Light{std::move(lightColor)}, direction{std::move(direction)} {}
};

#endif // LIGHT_C