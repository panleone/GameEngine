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

Shader::Shader(ShaderType shaderType) noexcept : shaderType{shaderType} {
  int glShaderType = GL_VERTEX_SHADER;
  if (shaderType == ShaderType::FRAGMENT) {
    glShaderType = GL_FRAGMENT_SHADER;
  } else if (shaderType == ShaderType::GEOMETRY) {
    glShaderType = GL_GEOMETRY_SHADER;
  }
  shaderID = glCreateShader(glShaderType);
}

Shader::~Shader() noexcept { glDeleteShader(shaderID); }

void Shader::compile(std::string_view shaderFile) const {
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

unsigned int Shader::getID() const { return this->shaderID; }

ShaderProgram::ShaderProgram(std::string_view vShaderFile,
                             std::string_view fShaderFile) {
  Shader vShader = Shader(ShaderType::VERTEX);
  vShader.compile(vShaderFile);

  Shader fShader = Shader(ShaderType::FRAGMENT);
  fShader.compile(fShaderFile);

  rawProgram = std::make_unique<RawShaderProgram>();
  unsigned int programID = rawProgram->getID();
  glAttachShader(programID, vShader.getID());
  glAttachShader(programID, fShader.getID());
  glLinkProgram(programID);
  glDetachShader(programID, vShader.getID());
  glDetachShader(programID, fShader.getID());

  int success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success)
    throw std::runtime_error("Cannot link the shader program!");
}

ShaderProgram::ShaderProgram(std::string_view vShaderFile,
                             std::string_view fShaderFile,
                             std::string_view gShaderFile) {
  Shader vShader = Shader(ShaderType::VERTEX);
  vShader.compile(vShaderFile);

  Shader gShader = Shader(ShaderType::GEOMETRY);
  gShader.compile(gShaderFile);

  Shader fShader = Shader(ShaderType::FRAGMENT);
  fShader.compile(fShaderFile);

  rawProgram = std::make_unique<RawShaderProgram>();
  unsigned int programID = rawProgram->getID();
  glAttachShader(programID, vShader.getID());
  glAttachShader(programID, gShader.getID());
  glAttachShader(programID, fShader.getID());
  glLinkProgram(programID);
  glDetachShader(programID, vShader.getID());
  glDetachShader(programID, gShader.getID());
  glDetachShader(programID, fShader.getID());

  int success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success)
    throw std::runtime_error("Cannot link the shader program!");
}

void ShaderProgram::use() const { rawProgram->use(); }

bool ShaderProgram::setTexture(TextureType textureType, int textureNumber,
                               int textureUnit) const {
  return setUniform(textureTypeConverter(textureType, textureNumber),
                    textureUnit);
}