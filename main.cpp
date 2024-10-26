#include "WindowManager.h"
#include "objects/cube/Cube.h"

std::unique_ptr<WindowManager> globalWindowManager =
    std::make_unique<WindowManager>(800, 600);

int main() {
  Cube cube{1};
  while (!globalWindowManager->shouldClose()) {
    glfwPollEvents();

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    cube.render();
    globalWindowManager->swapBuffers();
  }
}