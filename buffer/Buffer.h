#ifndef BUFFER_C
#define BUFFER_C

#include <cassert>
#include <glad/glad.h>

enum class BUFFER_TYPE {
  VAO,
  VBO,
  EBO,
  TEXTURE,
  FBO,
  RBO,
  // Not buffers but same formalism
  SHADER_PROGRAM,
  SHADER_VERTEX,
  SHADER_GEOMETRY,
  SHADER_FRAGMENT
};

/**
 * Wrapper class around OpenGL Buffers. Not copyable nor movable.
 * Wrap it around smart pointers to create copyable and movable versions.
 */
template <BUFFER_TYPE bufferType> class Buffer {
private:
  unsigned int bufferID;

public:
  Buffer();
  ~Buffer();
  Buffer(const Buffer &buffer) = delete;
  Buffer(Buffer &&buffer) = delete;
  unsigned int getID() const { return bufferID; };
  void bind() const;
};

template <BUFFER_TYPE bufferType> Buffer<bufferType>::Buffer() {
  if constexpr (bufferType == BUFFER_TYPE::VAO) {
    glGenVertexArrays(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::VBO) {
    glGenBuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::EBO) {
    glGenBuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::TEXTURE) {
    glGenTextures(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::FBO) {
    glGenFramebuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::RBO) {
    glGenRenderbuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_PROGRAM) {
    bufferID = glCreateProgram();
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_VERTEX) {
    bufferID = glCreateShader(GL_VERTEX_SHADER);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_GEOMETRY) {
    bufferID = glCreateShader(GL_GEOMETRY_SHADER);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_FRAGMENT) {
    bufferID = glCreateShader(GL_FRAGMENT_SHADER);
  } else {
    static_assert(false);
  }
}

template <BUFFER_TYPE bufferType> Buffer<bufferType>::~Buffer() {
  if constexpr (bufferType == BUFFER_TYPE::VAO) {
    glDeleteVertexArrays(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::VBO) {
    glDeleteBuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::EBO) {
    glDeleteBuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::TEXTURE) {
    glDeleteTextures(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::FBO) {
    glDeleteFramebuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::RBO) {
    glDeleteRenderbuffers(1, &bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_PROGRAM) {
    glDeleteProgram(bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_VERTEX) {
    glDeleteShader(bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_GEOMETRY) {
    glDeleteShader(bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_FRAGMENT) {
    glDeleteShader(bufferID);
  } else {
    static_assert(false);
  }
}

template <BUFFER_TYPE bufferType> void Buffer<bufferType>::bind() const {
  if constexpr (bufferType == BUFFER_TYPE::VAO) {
    glBindVertexArray(bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::VBO) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::EBO) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::TEXTURE) {
    glBindTexture(GL_TEXTURE_2D, bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::FBO) {
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::RBO) {
    glBindRenderbuffer(GL_RENDERBUFFER, bufferID);
  } else if constexpr (bufferType == BUFFER_TYPE::SHADER_PROGRAM) {
    glUseProgram(bufferID);
  } else {
    // Not bindable...
    static_assert(false);
  }
}

#endif // BUFFER_C