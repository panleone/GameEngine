#include "WindowManager.h"
#include "objects/cube/Cube.h"
#include "Camera.h"

std::unique_ptr<WindowManager> globalWindowManager =
    std::make_unique<WindowManager>(800, 600);

void processInput(Camera &camera, float deltaTime) {
  camera.setViewMatrix(globalWindowManager->mouseXOffset,
                       globalWindowManager->mouseYOffset);
  camera.setProjectiveMatrix(globalWindowManager->mouseScrollOffset,
                             globalWindowManager->getWindowAspectRatio());
  float dX = (globalWindowManager->isKeyPressed(GLFW_KEY_D) -
              globalWindowManager->isKeyPressed(GLFW_KEY_A)) *
             deltaTime;
  float dZ = (globalWindowManager->isKeyPressed(GLFW_KEY_W) -
              globalWindowManager->isKeyPressed(GLFW_KEY_S)) *
             deltaTime;
  camera.setCameraPos(dX, 0.0, dZ);
}
int main() {
  Camera camera{45};
  Cube cube{1.0f,
            {1.0f, 0.0f, -5.0f},
            {0.0f, 0.0f, 0.0f},
            mat::normalize(Vec3f{1.0f, 1.0f, 0.0f}),
            1.0f};

  float deltaTime;
  float lastFrame = glfwGetTime();

  globalWindowManager->disableMouseCursor();
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
    cube.update(deltaTime);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    cube.render(camera);
    globalWindowManager->swapBuffers();
  }
}