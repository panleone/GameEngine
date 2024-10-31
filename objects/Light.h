#ifndef LIGHT_C
#define LIGHT_C

#include "../math/Matrix.h"

class Light {
public:
  virtual ~Light() = default;
  virtual Vec3f getPosition() const = 0;
  virtual Vec3f ambientIntensity() const = 0;
  virtual Vec3f diffuseIntensity() const = 0;
  virtual Vec3f specularIntensity() const = 0;
};

#endif // LIGHT_C