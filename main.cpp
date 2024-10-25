#include "WindowManager.h"

std::unique_ptr<WindowManager> globalWindowManager =
    std::make_unique<WindowManager>(800, 600);

int main() {
  while (!globalWindowManager->shouldClose()) {
    glfwPollEvents();
  }
}