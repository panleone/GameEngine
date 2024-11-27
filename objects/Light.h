#ifndef LIGHT_C
#define LIGHT_C

#include "../math/Matrix.h"
#include "Entity.h"

class Light {
public:
  Vec3f lightColor;
  Vec3f ambientIntensity;
  Vec3f diffuseIntensity;
  Vec3f specularIntensity;
  Light(Light &&light) = default;
  Light(Vec3f lightColor) : lightColor{std::move(lightColor)} {
    // Default values that seems to give good results
    ambientIntensity = this->lightColor * 0.1f;
    diffuseIntensity = this->lightColor * 0.25f;
    specularIntensity = this->lightColor.clone();
  }
  virtual Vec4f getLightVector() const = 0;
  virtual ~Light() = default;
};

class PointLight : public Entity, public Light {
public:
  // By default set a realistic 1/d^2 attenuation
  Vec3f attenuationCoefficients = Vec3f{0.0f, 0.0f, 1.0f};
  PointLight(PointLight &&light) = default;
  PointLight(Model model, Vec3f lightColor)
      : Entity{std::move(model)}, Light{std::move(lightColor)} {};
  Vec4f getLightVector() const override {
    const Vec3f &pos = this->position;
    return Vec4f{pos(0), pos(1), pos(2), 1.0f};
  }
};

class DirectionalLight : public Light {
public:
  Vec3f direction;
  DirectionalLight(Vec3f direction, Vec3f lightColor)
      : Light{std::move(lightColor)}, direction{std::move(direction)} {}
  Vec4f getLightVector() const override {
    return Vec4f{direction(0), direction(1), direction(2), 0.0f};
  }
};

#endif // LIGHT_C