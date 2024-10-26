#include "WindowManager.h"
#include "objects/cube/Cube.h"
#include "Camera.h"

std::unique_ptr<WindowManager> globalWindowManager =
    std::make_unique<WindowManager>(800, 600);

int main() {
  Camera camera{45};
  Cube cube{1, {1.0f, 0.0f, 0.0f}};
  while (!globalWindowManager->shouldClose()) {
    // handle user inputs
    glfwPollEvents();
    camera.setViewMatrix(globalWindowManager->mouseXOffset,
                         globalWindowManager->mouseYOffset);
    camera.setProjectiveMatrix(globalWindowManager->mouseScrollOffset,
                               globalWindowManager->getWindowAspectRatio());
    globalWindowManager->resetOffests();

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    cube.render(camera);
    globalWindowManager->swapBuffers();
  }
}