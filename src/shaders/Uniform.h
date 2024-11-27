#ifndef UNIFORM_C
#define UNIFORM_C

#include <cassert>
#include <glad/glad.h>

#include "../math/Matrix.h"

// This class represents a uniform field of a shader.
template <typename T> class Uniform {
private:
  GLint location{-1};
  GLuint programID;
  std::string name;
  // TODO: add a comparison operator for vector/matrices of floats
  //  and implement a cache system (not compatible with copy ctor)
  // std::optional<T> cachedData{std::nullopt};
  void setUniformInternal(const T &data) const;
  void assertProgramInUse() const;

public:
  Uniform(GLuint programID, std::string name)
      : programID{programID}, name{std::move(name)} {
    location = glGetUniformLocation(programID, this->name.data());
    // there is no field named name in the shader
    assert(location != -1);
  };
  void setUniform(const T &data);
};

template <typename T> void Uniform<T>::setUniformInternal(const T &data) const {
  assertProgramInUse();
  // decay shouldn't be necessary...
  using dT = std::decay_t<T>;

  if constexpr (std::is_same_v<dT, int>) {
    glUniform1i(location, data);
  } else if constexpr (std::is_same_v<dT, float>) {
    glUniform1f(location, data);
  } else if constexpr (std::is_same_v<dT, Mat4f>) {
    glUniformMatrix4fv(location, 1, GL_FALSE, data.data().data());
  } else if constexpr (std::is_same_v<dT, Vec3f>) {
    glUniform3fv(location, 1, data.data().data());
  } else if constexpr (std::is_same_v<dT, Vec4f>) {
    glUniform4fv(location, 1, data.data().data());
  } else if constexpr (std::is_same_v<dT, std::size_t>) {
    glUniform1i(location, data);
  } else {
    static_assert(false);
  }
}

template <typename T> void Uniform<T>::setUniform(const T &data) {
  setUniformInternal(data);
}

template <typename T> void Uniform<T>::assertProgramInUse() const {
  GLint activeProgramID = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgramID);
  assert(programID == activeProgramID);
}
#endif // UNIFORM_C