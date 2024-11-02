#include "Cube.h"

#include <memory>
#include <ranges>

#include "../../math/MatrixUtils.h"
Cube::Cube(float size, Vec3f iPos, Vec3f iVel, Vec3f iAxis, float iAngVel,
           float theta)
    : Entity(std::move(iPos), std::move(iVel), std::move(iAxis), iAngVel,
             theta) {
  this->scale = size;
  std::vector<float> vertices = {
      1.0f, 1.0f, -1.0f, 0.0f, 1.0f,   // 0
      1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  // 1
      -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // 2
      -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  // 3
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f,    // 4
      1.0f, -1.0f, 1.0f, 1.0f, 0.0f,   // 5
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,  // 6
      -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,   // 7

      // Duplicated vertices to map texture coordinates
      -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 3' = 8
      -1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // 7' = 9

      -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // 2' = 10
      1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  // 1' = 11
  };
  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3, 4,  6, 5, 4,  7, 6,
                                       0, 9, 4, 0, 8, 9, 11, 5, 6, 11, 6, 10,
                                       0, 4, 1, 5, 1, 4, 7,  3, 2, 7,  2, 6};
  auto attrPointer = []() -> void {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  };
  this->model =
      std::make_unique<Model>(vertices, indices, std::move(attrPointer));
  this->program = std::make_unique<ShaderProgram>(
      "shaders/phong_light_model/phong_light.vs",
      "shaders/phong_light_model/phong_light.fs",
      "shaders/phong_light_model/phong_light.gs");

  this->model->addTexture(GL_TEXTURE0, "textures/container.png");
  program->setUniform("material.diffuse", 0);
  this->model->addTexture(GL_TEXTURE1, "textures/container_specular.png");
  program->setUniform("material.specular", 1);

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

static void handleLight(const Light *light, std::size_t i,
                        const ShaderProgram &program) {
  std::string uniformName = std::format("lights[{}]", i);
  program.setUniform(std::format("{}.position", uniformName),
                     light->getPosition());
  program.setUniform(std::format("{}.ambient", uniformName),
                     light->ambientIntensity());
  program.setUniform(std::format("{}.diffuse", uniformName),
                     light->diffuseIntensity());
  program.setUniform(std::format("{}.specular", uniformName),
                     light->specularIntensity());
  program.setUniform(std::format("{}.attenuation", uniformName),
                     light->attenuationCoefficients());
}

void Cube::handleLights(std::span<Light *> lights) {
  for (const auto &[i, light] : std::views::enumerate(lights)) {
    handleLight(light, i, *program);
  }
  program->setUniform("nLights", lights.size());
}
