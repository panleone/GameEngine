#include "Cube.h"

#include <memory>

#include "../../math/MatrixUtils.h"
Cube::Cube(float size, Vec3f iPos, Vec3f iVel, Vec3f iAxis, float iAngVel,
           float theta)
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
  this->program = std::make_unique<ShaderProgram>(
      "shaders/phong_light_model/phong_light.vs",
      "shaders/phong_light_model/phong_light.fs",
      "shaders/phong_light_model/phong_light.gs");

  program->setUniform("material.ambient", Vec3f{1.0f, 0.5f, 0.31f});
  program->setUniform("material.diffuse", Vec3f{1.0f, 0.5f, 0.31f});
  program->setUniform("material.specular", Vec3f{0.5f, 0.5f, 0.5f});
  program->setUniform("material.shininess", 32.0f);
}

void Cube::render(const Camera &camera) const {
  this->program->use();

  Mat4f pvMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
  program->setUniform("mMatrix", modelMatrix());
  program->setUniform("pvMatrix", pvMatrix);
  program->setUniform("eyePos", camera.getCameraPos());
  this->model->bindAndDraw();
}

void Cube::updateLight(const Light &light) {
  program->setUniform("light.position", light.getPosition());
  program->setUniform("light.ambient", light.ambientIntensity());
  program->setUniform("light.diffuse", light.diffuseIntensity());
  program->setUniform("light.specular", light.specularIntensity());
}
