#ifndef CAMERA_C
#define CAMERA_C

#include "math/Matrix.h"
#include "math/MatrixUtils.h"

class Camera {
private:
  // All angles (fov, phi, theta) are in degrees
  Mat4f projectionMatrix;
  float fov;

  Mat4f viewMatrix;
  float phi = 0;
  float theta = 0;
  Vec3f cameraUp{0.0f, 1.0f, 0.0f};
  Vec3f cameraPos{0.0f, 0.0f, 1.0f};
  Vec3f cameraFront{0.0f, 0.0f, -1.0f};

public:
  Camera(float fov)
      : viewMatrix{mat::identity()}, projectionMatrix{mat::identity()},
        fov{fov} {};
  const Mat4f &getProjectionMatrix() const;
  const Mat4f &getViewMatrix() const;
  void setProjectiveMatrix(float mouseScrollOffset, float aspectRatio);
  void setOrthographicMatrix(float r, float t);
  void setViewMatrix(float mouseXposOffset, float mouseYposOffset);
  void setCameraPos(float dX, float dY, float dZ);
};

#endif // CAMERA_C