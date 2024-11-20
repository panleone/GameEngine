#include "Shader.h"

static std::string fileToString(std::string_view fileLocation) {
  std::ifstream fStream;
  fStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  std::ostringstream stringStream;
  fStream.open(fileLocation.data());
  stringStream << fStream.rdbuf();
  return stringStream.str();
}

// Follow this convention when creating shaders
static std::string textureTypeConverter(TextureType textureType,
                                        int textureNumber) {
  switch (textureType) {
  case TextureType::DIFFUSE:
    return std::format("material.texture_diffuse{}", textureNumber);
  case TextureType::SPECULAR:
    return std::format("material.texture_specular{}", textureNumber);
  default:
    throw std::runtime_error("texture type not supported yet");
  }
}

void Shader::compile(std::string_view shaderFile) const {
  auto shaderID = rawShader.getID();
  std::string shaderSourceCode = fileToString(shaderFile);
  auto shaderSourceCodePtr = shaderSourceCode.data();
  glShaderSource(shaderID, 1, &shaderSourceCodePtr, nullptr);
  glCompileShader(shaderID);

  int success;
  char infoLog[512];
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderID, sizeof infoLog, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }
}

ShaderProgram::ShaderProgram(std::string_view vShaderFile,
                             std::string_view fShaderFile) {
  Shader vShader = Shader(BUFFER_TYPE::SHADER_VERTEX);
  vShader.compile(vShaderFile);

  Shader fShader = Shader(BUFFER_TYPE::SHADER_FRAGMENT);
  fShader.compile(fShaderFile);

  rawProgram = std::make_unique<Buffer>(BUFFER_TYPE::SHADER_PROGRAM);
  unsigned int programID = rawProgram->getID();
  glAttachShader(programID, vShader.rawShader.getID());
  glAttachShader(programID, fShader.rawShader.getID());
  glLinkProgram(programID);
  glDetachShader(programID, vShader.rawShader.getID());
  glDetachShader(programID, fShader.rawShader.getID());

  int success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success)
    throw std::runtime_error("Cannot link the shader program!");
}

ShaderProgram::ShaderProgram(std::string_view vShaderFile,
                             std::string_view fShaderFile,
                             std::string_view gShaderFile) {
  Shader vShader = Shader(BUFFER_TYPE::SHADER_VERTEX);
  vShader.compile(vShaderFile);

  Shader gShader = Shader(BUFFER_TYPE::SHADER_GEOMETRY);
  gShader.compile(gShaderFile);

  Shader fShader = Shader(BUFFER_TYPE::SHADER_FRAGMENT);
  fShader.compile(fShaderFile);

  rawProgram = std::make_unique<Buffer>(BUFFER_TYPE::SHADER_PROGRAM);
  unsigned int programID = rawProgram->getID();
  glAttachShader(programID, vShader.rawShader.getID());
  glAttachShader(programID, gShader.rawShader.getID());
  glAttachShader(programID, fShader.rawShader.getID());
  glLinkProgram(programID);
  glDetachShader(programID, vShader.rawShader.getID());
  glDetachShader(programID, gShader.rawShader.getID());
  glDetachShader(programID, fShader.rawShader.getID());

  int success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success)
    throw std::runtime_error("Cannot link the shader program!");
}

void ShaderProgram::use() const { rawProgram->bind(); }

bool ShaderProgram::setTexture(TextureType textureType, int textureNumber,
                               int textureUnit) const {
  return setUniform(textureTypeConverter(textureType, textureNumber),
                    textureUnit);
}