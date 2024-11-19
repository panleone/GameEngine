#include <filesystem>

#include "WindowManager.h"
#include "objects/Entity.h"
#include "objects/EntityManager.h"
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
  // 3) Square model
  std::string squarePath = getAbsPath("textures/window/square.obj");
  res.insert(std::make_pair("square", Model{squarePath}));
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

int main() {
  // Load models and create entities from them
  auto models = loadModels();

  // Build a scene
  Entity backpack(models.at("backpack"));
  backpack.position(2) = 2.0f;
  backpack.scale = 0.3f;
  Entity window1(models.at("square"));
  Entity window2(models.at("square"));
  window2.position(2) = 1.0f;
  Light light{models.at("cube"), Vec3f(1.0f, 1.0f, 1.0f)};
  light.position(1) = -0.5f;
  light.scale = 0.2f;

  EntityManager entityManager{loadShaders()};
  entityManager.addSolidEntity(&backpack);
  entityManager.addTransparentEntity(&window1);
  entityManager.addTransparentEntity(&window2);
  entityManager.addLight(&light);

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