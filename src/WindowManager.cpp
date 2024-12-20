#include "WindowManager.h"

#include <stdexcept>

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
  if (globalWindowManager) {
    globalWindowManager->screenWidth = width;
    globalWindowManager->screenHeight = height;
  }
}

static void scroll_callback(GLFWwindow *window, double xoffset,
                            double yoffset) {
  if (globalWindowManager)
    globalWindowManager->mouseScrollOffset += yoffset;
}

static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static bool mouseInitialized = false;
  static float oldMouseXpos = 0.0f;
  static float oldMouseYpos = 0.0f;

  if (!globalWindowManager)
    return;
  if (!mouseInitialized) {
    oldMouseXpos = xpos;
    oldMouseYpos = ypos;
    mouseInitialized = true;
  }
  globalWindowManager->mouseXOffset = xpos - oldMouseXpos;
  globalWindowManager->mouseYOffset = oldMouseYpos - ypos;
  oldMouseXpos = xpos;
  oldMouseYpos = ypos;
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

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
    glfwDestroyWindow(window);
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }
  glViewport(0, 0, screenWidth, screenHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  // gamma correction is done in the post processing stage
  glDisable(GL_FRAMEBUFFER_SRGB);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool WindowManager::created{false};

float WindowManager::getWindowAspectRatio() const {
  return this->screenWidth / this->screenHeight;
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}

void WindowManager::swapBuffers() const { glfwSwapBuffers(window); }

void WindowManager::resetOffests() {
  this->mouseScrollOffset = 0.0f;
  this->mouseXOffset = 0.0f;
  this->mouseYOffset = 0.0f;
}

bool WindowManager::isKeyPressed(int key) const {
  return glfwGetKey(window, key) == GLFW_PRESS;
}

void WindowManager::disableMouseCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::enableMouseCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}