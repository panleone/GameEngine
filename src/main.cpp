#include <filesystem>

#include "WindowManager.h"

#include "buffer/FrameBuffer.h"
#include "Camera.h"
#include "objects/Entity.h"
#include "objects/EntityManager.h"
#include "objects/Light.h"
#include "objects/Model.h"
#include "shaders/post_processing/PostProcessingShader.h"
#include "utils/Random.h"

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
  std::string backpackPath = getAbsPath("src/textures/backpack/backpack.obj");
  res.insert(std::make_pair("backpack", Model{backpackPath}));
  // 2) Cube model
  std::string cubePath = getAbsPath("src/textures/cube/cube.obj");
  res.insert(std::make_pair("cube", Model{cubePath}));
  // 3) Transparent window model
  std::string windowPath = getAbsPath("src/textures/window/square.obj");
  res.insert(std::make_pair("window", Model{windowPath}));
  // 4) Empty rectangle model for post processing
  std::string rectanglePath =
      getAbsPath("src/textures/rectangle/rectangle.obj");
  res.insert(std::make_pair("rectangle", Model{rectanglePath}));
  return res;
}

int main() {
  // Load models
  auto models = loadModels();

  // Load shaders
  EntityShader entityShader{};
  LightShader lightShader{};
  PostProcessingShader postProcessingShader{};

  // Build a scene
  Entity backpack(models.at("backpack"));
  backpack.setScale(0.3f);
  std::vector<PointLight> lights;
  const float R = 3.0f;
  for (int i = 0; i < 10; i++) {
    float r = rng::randomFloat(0.0f, 1.0f);
    float g = rng::randomFloat(0.0f, 1.0f);
    float b = rng::randomFloat(0.0f, 1.0f);
    PointLight light{models.at("cube"), Vec3f{r, g, b}};
    float theta = rng::randomAngle();
    float phi = rng::randomAngle();
    light.position = Vec3f{cos(phi) * sin(theta) * R, sin(phi) * sin(theta) * R,
                           R * cos(theta)};
    light.setScale(0.2f);
    lights.push_back(std::move(light));
  }
  DirectionalLight dirLight{Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.1f, 0.1f, 0.1f)};

  EntityManager entityManager{entityShader, lightShader};
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

  globalWindowManager->disableMouseCursor();
  globalWindowManager->resetTimer();
  float deltaTime;
  while (!globalWindowManager->shouldClose()) {
    // Compute the elapsed time
    deltaTime = globalWindowManager->elapsedTimeInSeconds();
    globalWindowManager->resetTimer();

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