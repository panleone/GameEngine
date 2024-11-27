#ifndef LIGHT_SHADER_C
#define LIGHT_SHADER_C

#include "../Uniform.h"
#include "../Shader.h"

// Binding class with light_source shader files
class LightShader : public ShaderProgram {
private:
  Uniform<Vec3f> lightColor{rawProgram->getID(), "lightColor"};
  Uniform<Mat4f> pvmMatrix{rawProgram->getID(), "pvmMatrix"};

public:
  LightShader()
      : ShaderProgram("src/shaders/light_source/light_source.vs",
                      "src/shaders/light_source/light_source.fs") {};

  void setPvmMatrix(const Mat4f &mat) { pvmMatrix.setUniform(mat); }
  void setLightColor(const Vec3f &color) { lightColor.setUniform(color); }
  bool setTexture(TextureType textureType, int textureNumber,
                  int textureUnit) override {
    return false;
  };
};

#endif // LIGHT_SHADER_C