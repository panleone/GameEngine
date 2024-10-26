#include "../../Camera.h"
#include "../Entity.h"

class Cube final : public Entity {
public:
  Cube(float size, Vec3f iPos = {0.0f, 0.0f, 0.0f},
       Vec3f iVel = {0.0f, 0.0f, 0.0f});
  void render(const Camera &camera) const override;
};