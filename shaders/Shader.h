#ifndef SHADER_C
#define SHADER_C

#include <fstream>
#include <glad/glad.h>
#include <memory>
#include <sstream>

#include "../buffer/Buffer.h"
#include "../math/Matrix.h"
#include "../textures/Texture.h"

class Shader {
public:
  explicit Shader(BUFFER_TYPE shaderType) noexcept : rawShader{shaderType} {};
  void compile(std::string_view shaderFile) const;
  Buffer rawShader;
};

class ShaderProgram {
public:
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile);
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile,
                std::string_view gShaderFile);
  void use() const;

  template <typename T>
  bool setUniform(std::string_view uniformName, T &&val) const {
    unsigned int programID = rawProgram->getID();
    auto uniformLocation = glGetUniformLocation(programID, uniformName.data());
    if (uniformLocation == -1) {
      return false;
    }
    // Fetch the program in use
    GLint activeProgramID = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgramID);
    rawProgram->bind();
    if constexpr (std::is_same_v<std::decay_t<T>, int>) {
      glUniform1i(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
      glUniform1f(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, Mat4f>) {
      glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, val.data().data());
    } else if constexpr (std::is_same_v<std::decay_t<T>, Vec3f>) {
      glUniform3fv(uniformLocation, 1, val.data().data());
    } else if constexpr (std::is_same_v<std::decay_t<T>, std::size_t>) {
      glUniform1i(uniformLocation, std::forward<T>(val));
    } else {
      static_assert(false);
    }
    // Reactivate the old program
    glUseProgram(activeProgramID);
    return true;
  }
  /**
   * Set a 2D texture in the shader.
   * @param textureType - type of the texture (Diffuse or Specular)
   * @param textureNumber - number of the texture: ex 1 for the first diffuse
   * @param textureUnit - texture unit in which the texture is activated
   * @return
   */
  bool setTexture(TextureType textureType, int textureNumber,
                  int textureUnit) const;

private:
  std::unique_ptr<Buffer> rawProgram;
};

#endif // SHADER_C