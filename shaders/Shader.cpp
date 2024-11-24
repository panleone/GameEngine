#include "Shader.h"
#include "ShaderCache.h"

static std::string fileToString(std::string_view fileLocation) {
  std::ifstream fStream;
  fStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  std::ostringstream stringStream;
  fStream.open(fileLocation.data());
  stringStream << fStream.rdbuf();
  return stringStream.str();
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

ShaderCache<int, std::string> diffuseShaderCache{[](int &k) {
  return std::string("material.texture_diffuse") + std::to_string(k);
}};
ShaderCache<int, std::string> specularShaderCache{[](int &k) {
  return std::string("material.texture_specular") + std::to_string(k);
}};

// Follow this convention when creating shaders
static const std::string &textureTypeConverter(TextureType textureType,
                                               int textureNumber) {
  switch (textureType) {
  case TextureType::DIFFUSE:
    return diffuseShaderCache.get(textureNumber);
  case TextureType::SPECULAR:
    return specularShaderCache.get(textureNumber);
  default:
    throw std::runtime_error("texture type not supported yet");
  }
}

bool ShaderProgram::setTexture(TextureType textureType, int textureNumber,
                               int textureUnit) const {
  return setUniform(textureTypeConverter(textureType, textureNumber),
                    textureUnit);
}

ShaderCache<int, std::vector<std::string>> lightEntityShaderCache{[](int &k) {
  std::string lightName = std::format("lights[{}]", k);
  std::vector<std::string> val(5);
  val.at(0) = lightName + std::string(".ambient");
  val.at(1) = lightName + std::string(".diffuse");
  val.at(2) = lightName + std::string(".specular");
  val.at(3) = lightName + std::string(".lightVector");
  val.at(4) = lightName + std::string(".attenuation");
  return val;
}};
bool ShaderProgram::addLightToEntityShader(const LightBinding &light,
                                           int lightNumber) {
  const auto &names = lightEntityShaderCache.get(lightNumber);
  bool res = true;
  res &= setUniform(names.at(0), *light.ambient);
  res &= setUniform(names.at(1), *light.diffuse);
  res &= setUniform(names.at(2), *light.specular);
  res &= setUniform(names.at(3), light.lightVector);
  // Only point light has an attenuation vector
  if (light.attenuation) {
    res &= setUniform(names.at(4), *light.attenuation);
  }
  return res;
}

bool ShaderProgram::setPostProcessing(int textureUnit) const {
  return setUniform("postProcessing", textureUnit);
}