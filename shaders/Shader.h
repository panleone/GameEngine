#ifndef SHADER_C
#define SHADER_C

#include <fstream>
#include <glad/glad.h>
#include <memory>
#include <sstream>

#include "../buffer/Buffer.h"
#include "../math/Matrix.h"
#include "../textures/Texture.h"

class LightBinding;

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
    // Assert that the program is active
    assertProgramInUse();
    if constexpr (std::is_same_v<std::decay_t<T>, int>) {
      glUniform1i(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
      glUniform1f(uniformLocation, std::forward<T>(val));
    } else if constexpr (std::is_same_v<std::decay_t<T>, Mat4f>) {
      glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, val.data().data());
    } else if constexpr (std::is_same_v<std::decay_t<T>, Vec3f>) {
      glUniform3fv(uniformLocation, 1, val.data().data());
    } else if constexpr (std::is_same_v<std::decay_t<T>, Vec4f>) {
      glUniform4fv(uniformLocation, 1, val.data().data());
    } else if constexpr (std::is_same_v<std::decay_t<T>, std::size_t>) {
      glUniform1i(uniformLocation, std::forward<T>(val));
    } else {
      static_assert(false);
    }
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
  /**
   * Set a post processing color buffer in the specified textureUnit
   */
  bool setPostProcessing(int textureUnit) const;

  // TODO: Split shader.h in multiple files
  bool addLightToEntityShader(const LightBinding &light, int lightNumber);

private:
  std::unique_ptr<Buffer> rawProgram;
  void assertProgramInUse() const;
};

// Middle ground between the Light C++ class (See Light.h)
// and the GLSL light struct (See phong_light.fs)
class LightBinding {
public:
  Vec4f lightVector;
  const Vec3f *ambient{nullptr};
  const Vec3f *diffuse{nullptr};
  const Vec3f *specular{nullptr};
  const Vec3f *attenuation{nullptr};
  LightBinding(Vec4f lightVector) : lightVector{std::move(lightVector)} {};
  LightBinding(LightBinding &&lightBinding) = default;
  LightBinding(const LightBinding &lightBinding) = delete;
};

#endif // SHADER_C