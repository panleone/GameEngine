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
  Light *lightSource;

public:
  void addEntity(Entity *entity) { entities.push_back(entity); };
  void setLightSource(Light *source) { lightSource = source; };
  void update(float deltaTime) {
    for (Entity *entity : entities) {
      entity->update(deltaTime);
    }
    for (Entity *entity : entities) {
      entity->updateLight(*lightSource);
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
  CubicLight lightSource{0.2f, {0.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 0.0f}};

  entityManager.addEntity(&cube);
  entityManager.addEntity(&lightSource);
  entityManager.setLightSource(&lightSource);

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
    lightSource.velocity(0) = 2 * cos(glfwGetTime() - startTime);
    lightSource.velocity(2) = 2 * sin(glfwGetTime() - startTime);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    entityManager.render(camera);
    globalWindowManager->swapBuffers();
  }
}