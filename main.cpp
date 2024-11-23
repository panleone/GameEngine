#include <filesystem>
#include <chrono>
#include <thread>

#include "WindowManager.h"
#include "buffer/FrameBuffer.h"
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
  // 3) Transparent window model
  std::string windowPath = getAbsPath("textures/window/square.obj");
  res.insert(std::make_pair("window", Model{windowPath}));
  // 4) Empty rectangle model for post processing
  std::string rectanglePath = getAbsPath("textures/rectangle/rectangle.obj");
  res.insert(std::make_pair("rectangle", Model{rectanglePath}));
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

  // 3) Shader for post-processing
  ShaderProgram postProcessingShader(
      "shaders/post_processing/post_processing.vs",
      "shaders/post_processing/post_processing.fs");
  res.insert(
      std::make_pair("post_processing", std::move(postProcessingShader)));
  return res;
}

int main() {
  // Load models and create entities from them
  auto models = loadModels();
  auto shaders = loadShaders();

  // Build a scene
  Entity backpack(models.at("backpack"));
  backpack.scale = 0.3f;
  std::vector<PointLight> lights;
  for (int i = 0; i < 10; i++) {
    const float R = 3.0f;
    // Weird way to generate "random" numbers
    // TODO: write a RNG class
    float t = glfwGetTime() * 10000.0f;
    float t2 = (t + 0.1f) * 10000.0f;
    PointLight light{models.at("cube"), Vec3f{sin(t), cos(t), sin(2.0f * t)}};
    light.position =
        Vec3f{cos(t) * sin(t2) * R, sin(t) * sin(t2) * R, R * cos(t2)};
    light.scale = 0.2f;
    lights.push_back(std::move(light));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  DirectionalLight dirLight{Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.1f, 0.1f, 0.1f)};

  EntityManager entityManager{shaders};
  entityManager.addSolidEntity(&backpack);
  for (auto &light : lights) {
    entityManager.addPointLight(&light);
  }
  entityManager.setDirectionalLight(&dirLight);

  Camera camera{45, Vec3f{0.0f, 0.0f, 3.0f}};

  // Create the post-processing variables
  FrameBuffer postProcessingFBO{globalWindowManager->screenWidth,
                                globalWindowManager->screenHeight};
  Entity postProcessingTarget(models.at("rectangle"));
  auto &postProcessingShader = shaders.at("post_processing");

  globalWindowManager->disableMouseCursor();
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
    glDisable(GL_FRAMEBUFFER_SRGB);
    postProcessingFBO.bind();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    entityManager.render(camera);

    // post processing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    postProcessingShader.use();
    glActiveTexture(GL_TEXTURE0);
    postProcessingFBO.bindColor();
    postProcessingShader.setPostProcessing(0);
    postProcessingTarget.render(postProcessingShader);
    globalWindowManager->swapBuffers();
  }
}