#include <filesystem>
#include <map>
#include <cassert>

#include <vector>
#include <ranges>

#include "WindowManager.h"
#include "objects/Entity.h"
#include "objects/Light.h"
#include "Camera.h"

#include "objects/Model.h"

std::unique_ptr<WindowManager> globalWindowManager =
    std::make_unique<WindowManager>(800, 600);

void processInput(Camera &camera, float deltaTime) {
  camera.setViewMatrix(globalWindowManager->mouseXOffset,
                       globalWindowManager->mouseYOffset);
  camera.setProjectiveMatrix(globalWindowManager->mouseScrollOffset,
                             globalWindowManager->getWindowAspectRatio());
  const float speed = 5.0f;
  float dX = (globalWindowManager->isKeyPressed(GLFW_KEY_D) -
              globalWindowManager->isKeyPressed(GLFW_KEY_A)) *
             deltaTime;
  float dZ = (globalWindowManager->isKeyPressed(GLFW_KEY_W) -
              globalWindowManager->isKeyPressed(GLFW_KEY_S)) *
             deltaTime;
  camera.setCameraPos(speed * dX, 0.0, speed * dZ);
}

std::string getAbsPath(std::string_view relPath) {
  std::string currDir = std::filesystem::current_path().string();
  return std::format("{}/{}", currDir, relPath);
}

// TODO: refactor in a class ModelLoader once there are more models
std::map<std::string, Model> loadModels() {
  std::map<std::string, Model> res;
  // 1) Backpack model
  std::string backpackPath = getAbsPath("textures/backpack/backpack.obj");
  res.insert(std::make_pair("backpack", Model{backpackPath}));
  // 2) Cube model
  std::string cubePath = getAbsPath("textures/cube/cube.obj");
  res.insert(std::make_pair("cube", Model{cubePath}));
  return res;
}

std::map<std::string, ShaderProgram> loadShaders() {
  std::map<std::string, ShaderProgram> res;
  // 1) Default shader for entities
  ShaderProgram entityShader("shaders/phong_light_model/phong_light.vs",
                             "shaders/phong_light_model/phong_light.fs",
                             "shaders/phong_light_model/phong_light.gs");
  res.insert(std::make_pair("entity", std::move(entityShader)));
  // 2) Shader for light sources
  ShaderProgram lightShader("shaders/light_source/light_source.vs",
                            "shaders/light_source/light_source.fs");
  res.insert(std::make_pair("light", std::move(lightShader)));

  // 3) Debug shader for visualizing normal vectors
  ShaderProgram normalShader("shaders/normal_visualizer/normal_visualizer.vs",
                             "shaders/normal_visualizer/normal_visualizer.fs",
                             "shaders/normal_visualizer/normal_visualizer.gs");
  res.insert(std::make_pair("normal", std::move(normalShader)));
  return res;
}

void setUpLight(Light &light) {
  light.ambientIntensity = light.lightColor * 0.1f;
  light.diffuseIntensity = light.lightColor * 0.25f;
  light.specularIntensity = light.lightColor.clone();
  light.attenuationCoefficients = Vec3f{1.0f, 0.09f, 0.032f};
}

// TODO: move in another file
class EntityManager {
  std::vector<Entity *> entities;
  std::vector<Light *> lights;
  std::map<std::string, ShaderProgram> shaders;

public:
  EntityManager(std::map<std::string, ShaderProgram> shaders)
      : shaders{std::move(shaders)} {};
  void addEntity(Entity *entity) { entities.push_back(entity); };
  void addLight(Light *source) {
    lights.push_back(source);
    auto &entityShader = shaders.at("entity");
    entityShader.use();
    std::string uniformName = std::format("lights[{}]", lights.size() - 1);
    entityShader.setUniform(std::format("{}.ambient", uniformName),
                            source->ambientIntensity);
    entityShader.setUniform(std::format("{}.diffuse", uniformName),
                            source->diffuseIntensity);
    entityShader.setUniform(std::format("{}.specular", uniformName),
                            source->specularIntensity);
    entityShader.setUniform(std::format("{}.attenuation", uniformName),
                            source->attenuationCoefficients);
    entityShader.setUniform("nLights", lights.size());
  };
  void update(float deltaTime) {
    for (Entity *entity : entities) {
      entity->update(deltaTime);
    }
    for (Light *light : lights) {
      light->update(deltaTime);
    }
    // Update light positions in the entity shader
    auto &entityShader = shaders.at("entity");
    entityShader.use();
    for (const auto &[i, light] : std::views::enumerate(lights)) {
      std::string uniformName = std::format("lights[{}]", i);
      entityShader.setUniform(std::format("{}.position", uniformName),
                              light->position);
    }
  }
  void render(const Camera &camera) {
    auto &entityShader = shaders.at("entity");
    entityShader.use();
    auto pvMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
    entityShader.setUniform("pvMatrix", pvMatrix);
    entityShader.setUniform("eyePos", camera.getCameraPos());
    for (const Entity *entity : entities) {
      entityShader.setUniform("mMatrix", entity->modelMatrix());
      entity->render(entityShader);
    }
    auto &lightShader = shaders.at("light");
    lightShader.use();
    for (const Light *light : lights) {
      lightShader.setUniform("lightColor", light->lightColor);
      lightShader.setUniform("pvmMatrix", camera.getProjectionMatrix() *
                                              camera.getViewMatrix() *
                                              light->modelMatrix());
      light->render(lightShader);
    }
  }
};
int main() {
  EntityManager entityManager{loadShaders()};
  // Load models and create entities from them
  auto models = loadModels();

  Entity backpack(models.at("backpack"));
  backpack.rotationAxis = Vec3f(0.0f, 0.0f, 1.0f);
  backpack.angularVelocity = 1.0f;
  entityManager.addEntity(&backpack);

  Light light{models.at("cube"), Vec3f(1.0f, 0.0f, 0.0f)};
  light.position(0) = -4.0f;
  light.scale = 0.2f;
  setUpLight(light);
  Light light2{models.at("cube"), Vec3f(0.0f, 1.0f, 0.0f)};
  light2.position(0) = 4.0f;
  light2.scale = 0.2f;
  setUpLight(light2);
  Light light3{models.at("cube"), Vec3f(1.0f, 1.0f, 1.0f)};
  light3.position(1) = 4.0f;
  light3.scale = 0.2f;
  setUpLight(light3);
  Light light4{models.at("cube"), Vec3f(0.0f, 0.0f, 1.0f)};
  light4.position(1) = -4.0f;
  light4.scale = 0.2f;
  setUpLight(light4);
  entityManager.addLight(&light);
  entityManager.addLight(&light2);
  entityManager.addLight(&light3);
  entityManager.addLight(&light4);

  Camera camera{45, Vec3f{0.0f, 3.0f, 3.0f}};

  globalWindowManager->disableMouseCursor();
  glEnable(GL_DEPTH_TEST);

  float deltaTime;
  float lastFrame = glfwGetTime();
  float startTime = glfwGetTime();
  while (!globalWindowManager->shouldClose()) {
    // Compute the elapsed time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // handle user inputs
    glfwPollEvents();
    processInput(camera, deltaTime);
    globalWindowManager->resetOffests();

    // update Entities
    entityManager.update(deltaTime);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    entityManager.render(camera);
    globalWindowManager->swapBuffers();
  }
}