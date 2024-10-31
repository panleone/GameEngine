#include "CubicLight.h"

#include <memory>

#include "../../math/MatrixUtils.h"
CubicLight::CubicLight(float size, Vec3f iPos, Vec3f iVel, Vec3f iAxis,
                       float iAngVel, float theta)
    : Entity(std::move(iPos), std::move(iVel), std::move(iAxis), iAngVel,
             theta) {
  this->scale = size;
  std::vector<float> vertices = {
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
  };
  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,
                                       0, 7, 4, 0, 3, 7, 1, 5, 6, 1, 6, 2,
                                       0, 4, 1, 5, 1, 4, 7, 3, 2, 7, 2, 6};
  auto attrPointer = []() -> void {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
  };
  this->model =
      std::make_unique<Model>(vertices, indices, std::move(attrPointer));
  this->program =
      std::make_unique<ShaderProgram>("shaders/light_source/light_source.vs",
                                      "shaders/light_source/light_source.fs");
  this->program->setUniform("lightColor", this->lightColor);
}

void CubicLight::render(const Camera &camera) const {
  this->program->use();

  Mat4f pvmMatrix =
      camera.getProjectionMatrix() * camera.getViewMatrix() * modelMatrix();
  program->setUniform("pvmMatrix", pvmMatrix);
  this->model->bindAndDraw();
}

Vec3f CubicLight::getPosition() const { return this->position.clone(); }
Vec3f CubicLight::ambientIntensity() const { return this->lightColor * 0.2f; }
Vec3f CubicLight::diffuseIntensity() const { return lightColor * 0.2f; }
Vec3f CubicLight::specularIntensity() const { return lightColor.clone(); }
