#ifndef SHADER_C
#define SHADER_C

#include <fstream>
#include <glad/glad.h>
#include <sstream>

#include "../math/Matrix.h"

enum class ShaderType { VERTEX, FRAGMENT, GEOMETRY };

class Shader {
private:
  ShaderType shaderType;
  unsigned int shaderId;

public:
  explicit Shader(ShaderType shaderType) noexcept;
  ~Shader() noexcept;
  Shader(const Shader &shader) = delete;
  Shader &operator=(const Shader &shader) = delete;

  void compile(std::string_view shaderFile) const;
  unsigned int getId() const;
};

class ShaderProgram {
public:
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile);
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile,
                std::string_view gShaderFile);
  void use() const;

  template <typename T>
  void setUniform(std::string_view uniformName, T &&val) const {
    auto uniformLocation = glGetUniformLocation(programId, uniformName.data());
    if (uniformLocation == -1) {
      throw std::runtime_error("Cannot find the uniform location!");
    }
    // Fetch the program in use
    GLint activeProgramId = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgramId);
    glUseProgram(programId);
    if constexpr (std::is_same_v<std::decay_t<T>, int>) {
      glUniform1i(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
      glUniform1f(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, Mat4f>) {
      glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, val.data().data());
    } else {
      static_assert(false);
    }
    // Reactivate the old program
    glUseProgram(activeProgramId);
  }

private:
  unsigned int programId;
};

#endif // SHADER_C