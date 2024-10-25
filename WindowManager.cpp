#include "WindowManager.h"

#include <stdexcept>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  if (globalWindowManager) {
    globalWindowManager->screenWidth = width;
    globalWindowManager->screenHeight = height;
  }
}

WindowManager::~WindowManager() { glfwTerminate(); }

WindowManager::WindowManager(float screenWidth, float screenHeight)
    : screenWidth{screenWidth}, screenHeight{screenHeight} {
  if (created)
    throw std::runtime_error("WindowManager already instantiated!");
  created = true;
  if (!glfwInit()) {
    throw std::runtime_error("Couldn't initialize GLFW!");
  }
  glfwWindowHint(GLFW_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_VERSION_MINOR, 3);
  window = glfwCreateWindow(screenWidth, screenHeight, "GameEngine", nullptr,
                            nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Couldn't create the window!");
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }
  glViewport(0, 0, screenWidth, screenHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool WindowManager::created{false};

float WindowManager::getWindowAspectRatio() const {
  return this->screenWidth / this->screenHeight;
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}