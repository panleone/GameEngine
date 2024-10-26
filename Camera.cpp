#include "Camera.h"

const Mat4f &Camera::getProjectionMatrix() const { return projectionMatrix; }
const Mat4f &Camera::getViewMatrix() const { return viewMatrix; }
void Camera::setProjectiveMatrix(float mouseScrollOffset, float aspectRatio) {
  fov -= mouseScrollOffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;

  projectionMatrix =
      mat::perspectiveProjection(mat::toRads(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::setOrthographicMatrix(float r, float t) {
  projectionMatrix = mat::orthographicProjection(r, t, 0.1, 100.0);
}

void Camera::setViewMatrix(float mouseXposOffset, float mouseYposOffset) {
  phi += mouseXposOffset * 0.1;
  theta += mouseYposOffset * 0.1;
  // spherical coordinates usual bounds
  if (theta > 89.0f) {
    theta = 89.0f;
  }
  if (theta < -89.0f) {
    theta = -89.0f;
  }
  float theta_rad = mat::toRads(theta);
  float phi_rad = mat::toRads(phi);

  cameraFront(0) = cos(theta_rad) * sin(phi_rad);
  cameraFront(1) = sin(theta_rad);
  cameraFront(2) = -cos(theta_rad) * cos(phi_rad);
  viewMatrix = mat::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}