#ifndef MATRIX_UTILS_C
#define MATRIX_UTILS_C

#include "Matrix.h"

namespace mat {
inline float distance2(const Vec3f &v1, const Vec3f &v2) {
  Vec3f v3 = v1 - v2;
  return v3.norm2();
}
// Returns the identity matrix
inline Mat4f identity() {
  Mat4f id;
  id(0, 0) = id(1, 1) = id(2, 2) = id(3, 3) = 1.0;
  return id;
}
template <typename T> [[nodiscard]] inline T normalize(T mat) {
  mat /= mat.norm();
  return mat;
}
// Returns the translation matrix associated to translationVector
inline Mat4f translate(const Vec3f &translationVector) {
  Mat4f translationMatrix = identity();
  for (int i = 0; i < 3; i++) {
    translationMatrix(i, 3) = translationVector(i);
  }
  return translationMatrix;
}
// Returns the scaling matrix associated to scalingVector
inline Mat4f scale(const Vec3f &scalingVector) {
  Mat4f scalingMatrix = identity();
  for (int i = 0; i < 3; i++) {
    scalingMatrix(i, i) = scalingVector(i);
  }
  return scalingMatrix;
}
// Returns the rotation matrix of angle theta around the versor direction
inline Mat4f rotate(float theta, const Vec3f &direction) {
  float ux = direction(0);
  float uy = direction(1);
  float uz = direction(2);
  float ct = std::cos(theta);
  float st = std::sin(theta);

  Mat4f rotationMatrix = identity();
  // See
  // https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
  rotationMatrix(0, 0) = ux * ux * (1 - ct) + ct;
  rotationMatrix(0, 1) = ux * uy * (1 - ct) - uz * st;
  rotationMatrix(0, 2) = ux * uz * (1 - ct) + uy * st;

  rotationMatrix(1, 0) = ux * uy * (1 - ct) + uz * st;
  rotationMatrix(1, 1) = uy * uy * (1 - ct) + ct;
  rotationMatrix(1, 2) = uy * uz * (1 - ct) - ux * st;

  rotationMatrix(2, 0) = ux * uz * (1 - ct) - uy * st;
  rotationMatrix(2, 1) = uy * uz * (1 - ct) + ux * st;
  rotationMatrix(2, 2) = uz * uz * (1 - ct) + ct;

  return rotationMatrix;
}

// Returns cross product between v1 and v2
inline Vec3f cross(const Vec3f &v1, const Vec3f &v2) {
  return Vec3f(v1(1) * v2(2) - v1(2) * v2(1), v1(2) * v2(0) - v1(0) * v2(2),
               v1(0) * v2(1) - v1(1) * v2(0));
}
// Implements the "View Matrix step" of the PVM change of coordinate
// See https://www.songho.ca/opengl/gl_camera.html#lookat
// eyePosition: position of the eye (camera)
// targetPosition: position at which the eye is looking
// upVector: uniquely determine the cartesian axis of the eye
inline Mat4f lookAt(const Vec3f &eyePosition, const Vec3f &targetPosition,
                    const Vec3f &upVector) {
  // z-axis of the eye coordinate system
  Vec3f eyeDirection = normalize(eyePosition - targetPosition);

  // x-axis of the eye coordinate system
  Vec3f eyeRight = normalize(cross(upVector, eyeDirection));

  // y-axis of the eye coordinate system
  Vec3f eyeUp = normalize(cross(eyeDirection, eyeRight));

  // To implement the change of coordinates first translate the eye in the
  // origin
  Mat4f translationEye = translate(-eyePosition);
  // Then apply a rotation that maps the old coordinate system (x,y,z) in the
  // new one (eyeRight, eyeUp, eyeDirection)
  Mat4f rotationEye = identity();
  for (size_t j = 0; j < 3; j++) {
    rotationEye(0, j) = eyeRight(j);
    rotationEye(1, j) = eyeUp(j);
    rotationEye(2, j) = eyeDirection(j);
  }
  return rotationEye * translationEye;
}

// Implements the most generic "Projection Matrix step" of the PVM change of
// coordinate, Perspective is taken in account See
// https://www.songho.ca/opengl/gl_projectionmatrix.html
inline Mat4f perspectiveProjection(float r, float l, float b, float t, float n,
                                   float f) {
  Mat4f res;
  res(0, 0) = 2 * n / (r - l);
  res(0, 2) = (r + l) / (r - l);

  res(1, 1) = 2 * n / (t - b);
  res(1, 2) = (t + b) / (t - b);

  res(2, 2) = -(f + n) / (f - n);
  res(2, 3) = -2 * f * n / (f - n);

  res(3, 2) = -1;
  return res;
}

// Overload in the special symmetric case
// r = -l
// t = -b
// parameterized by the fov angle and the aspect ratio
inline Mat4f perspectiveProjection(float fov, float aspectRatio, float n,
                                   float f) {
  float tangent = tan(fov / 2);
  float t = n * tangent;
  float r = t * aspectRatio;
  return perspectiveProjection(r, -r, -t, t, n, f);
}

// Implements the most generic "Projection Matrix step" of the PVM change of
// coordinate, Perspective is NOT taken in account See
// https://www.songho.ca/opengl/gl_projectionmatrix.html
inline Mat4f orthographicProjection(float r, float l, float b, float t, float n,
                                    float f) {
  Mat4f res = identity();
  res(0, 0) = 2 / (r - l);
  res(0, 3) = -(r + l) / (r - l);

  res(1, 1) = 2 / (t - b);
  res(1, 3) = -(t + b) / (t - b);

  res(2, 2) = -2 / (f - n);
  res(2, 3) = -(f + n) / (f - n);

  return res;
}
// Overload in the special symmetric case
// r = -l
// t = -b
inline Mat4f orthographicProjection(float r, float t, float n, float f) {
  return orthographicProjection(r, -r, -t, t, n, f);
}

inline float toRads(float degrees) { return degrees * (M_PI / 180); }
} // namespace mat

#endif // MATRIX_UTILS_C