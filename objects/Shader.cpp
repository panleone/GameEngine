#include "Shader.h"

static std::string fileToString(std::string_view fileLocation) {
  std::ifstream fStream;
  fStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  std::ostringstream stringStream;
  fStream.open(fileLocation.data());
  stringStream << fStream.rdbuf();
  return stringStream.str();
}

Shader::Shader(ShaderType shaderType) noexcept : shaderType{shaderType} {
  int glShaderType = GL_VERTEX_SHADER;
  if (shaderType == ShaderType::FRAGMENT) {
    glShaderType = GL_FRAGMENT_SHADER;
  } else if (shaderType == ShaderType::GEOMETRY) {
    glShaderType = GL_GEOMETRY_SHADER;
  }
  shaderId = glCreateShader(glShaderType);
}

Shader::~Shader() noexcept { glDeleteShader(shaderId); }

void Shader::compile(std::string_view shaderFile) const {
  std::string shaderSourceCode = fileToString(shaderFile);
  auto shaderSourceCodePtr = shaderSourceCode.data();
  glShaderSource(shaderId, 1, &shaderSourceCodePtr, nullptr);
  glCompileShader(shaderId);

  int success;
  char infoLog[512];
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderId, sizeof infoLog, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }
}

unsigned int Shader::getId() const { return this->shaderId; }

ShaderProgram::ShaderProgram(std::string_view vShaderFile,
                             std::string_view fShaderFile) {
  Shader vShader = Shader(ShaderType::VERTEX);
  vShader.compile(vShaderFile);

  Shader fShader = Shader(ShaderType::FRAGMENT);
  fShader.compile(fShaderFile);

  programId = glCreateProgram();
  glAttachShader(programId, vShader.getId());
  glAttachShader(programId, fShader.getId());
  glLinkProgram(programId);
  glDetachShader(programId, vShader.getId());
  glDetachShader(programId, fShader.getId());

  int success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
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

  programId = glCreateProgram();
  glAttachShader(programId, vShader.getId());
  glAttachShader(programId, gShader.getId());
  glAttachShader(programId, fShader.getId());
  glLinkProgram(programId);
  glDetachShader(programId, vShader.getId());
  glDetachShader(programId, gShader.getId());
  glDetachShader(programId, fShader.getId());

  int success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (!success)
    throw std::runtime_error("Cannot link the shader program!");
}

void ShaderProgram::use() const { glUseProgram(programId); }
