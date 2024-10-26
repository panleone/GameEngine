#include "Cube.h"

#include <memory>

#include "../../math/MatrixUtils.h"
Cube::Cube(float size, Vec3f iPos, Vec3f iVel, Vec3f iAxis, float iAngVel,
           float theta)
    : Entity(std::move(iPos), std::move(iVel), std::move(iAxis), iAngVel,
             theta) {
  this->scale = size;
  std::vector<float> vertices = {
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,

      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
  };
  std::vector<unsigned int> indices = {
      0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
      12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23,
  };
  auto attrPointer = []() -> void {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
  };
  this->model =
      std::make_unique<Model>(vertices, indices, std::move(attrPointer));
  this->program = std::make_unique<ShaderProgram>("objects/cube/Cube.vs",
                                                  "objects/cube/Cube.fs");
}

void Cube::render(const Camera &camera) const {
  this->program->use();

  Mat4f modelMatrix = mat::translate(position) *
                      mat::scale(Vec3f{scale, scale, scale}) *
                      mat::rotate(theta, rotationAxis.clone());
  Mat4f pvmMatrix =
      camera.getProjectionMatrix() * camera.getViewMatrix() * modelMatrix;
  program->setUniform("pvmMatrix", pvmMatrix);
  this->model->bindAndDraw();
};
