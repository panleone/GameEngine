#ifndef SHADER_C
#define SHADER_C

#include <fstream>
#include <memory>
#include <sstream>

#include "../buffer/Buffer.h"
#include "../textures/Texture.h"

class ShaderProgram {
public:
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile);
  ShaderProgram(std::string_view vShaderFile, std::string_view fShaderFile,
                std::string_view gShaderFile);
  virtual ~ShaderProgram() = default;
  void use() const;
  virtual bool setTexture(TextureType textureType, int textureNumber,
                          int textureUnit) = 0;

protected:
  std::unique_ptr<Buffer<BUFFER_TYPE::SHADER_PROGRAM>> rawProgram =
      std::make_unique<Buffer<BUFFER_TYPE::SHADER_PROGRAM>>();
};

#endif // SHADER_C