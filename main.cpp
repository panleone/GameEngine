#include <vector>

#include "WindowManager.h"
#include "objects/cube/Cube.h"
#include "objects/cubic_light_source/CubicLight.h"
#include "Camera.h"

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
class EntityManager {
  std::vector<Entity *> entities;
  std::vector<Light *> lights;

public:
  void addEntity(Entity *entity) { entities.push_back(entity); };
  void addLight(Light *source) { lights.push_back(source); };
  void update(float deltaTime) {
    for (Entity *entity : entities) {
      entity->update(deltaTime);
    }
    for (Entity *entity : entities) {
      entity->handleLights(std::span<Light *>{lights});
    }
  }
  void render(const Camera &camera) {
    for (const Entity *entity : entities) {
      entity->render(camera);
    }
  }
};
int main() {
  EntityManager entityManager;
  Camera camera{45, Vec3f{0.0f, 0.0f, 8.0f}};
  Cube cube{
      1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 1.0f};
  CubicLight lightSource1{0.2f, {0.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 0.0f}};
  CubicLight lightSource2{0.2f, {0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f}};
  CubicLight lightSource3{0.2f, {2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  CubicLight lightSource4{0.2f, {-2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  lightSource2.setColor({0.0f, 1.0f, 0.0f});
  lightSource3.setColor({1.0f, 0.0f, 0.0f});
  lightSource4.setColor({0.0f, 0.0f, 1.0f});

  entityManager.addEntity(&cube);
  entityManager.addEntity(&lightSource1);
  entityManager.addLight(&lightSource1);
  entityManager.addEntity(&lightSource2);
  entityManager.addLight(&lightSource2);
  entityManager.addEntity(&lightSource3);
  entityManager.addLight(&lightSource3);
  entityManager.addEntity(&lightSource4);
  entityManager.addLight(&lightSource4);

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