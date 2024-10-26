#ifndef WindowManager_C
#define WindowManager_C

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class WindowManager {
private:
  static bool created;
  GLFWwindow *window;

public:
  // Screen variables
  float screenWidth;
  float screenHeight;

  WindowManager(const WindowManager &wManager) = delete;
  WindowManager(float screenWidth, float screenHeight);
  ~WindowManager();
  float getWindowAspectRatio() const;
  bool shouldClose() const;
  void swapBuffers() const;
};

// Global Singleton WindowManager
extern std::unique_ptr<WindowManager> globalWindowManager;
#endif // WindowManager_C