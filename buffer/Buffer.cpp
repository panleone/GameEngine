#include "Buffer.h"

#include <cassert>
#include <glad/glad.h>

Buffer::Buffer(BUFFER_TYPE type) : type{type} {
  switch (type) {
  case BUFFER_TYPE::VAO:
    glGenVertexArrays(1, &bufferID);
    break;
  case BUFFER_TYPE::VBO:
    glGenBuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::EBO:
    glGenBuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::TEXTURE:
    glGenTextures(1, &bufferID);
    break;
  case BUFFER_TYPE::FBO:
    glGenFramebuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::RBO:
    glGenRenderbuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::SHADER_PROGRAM:
    bufferID = glCreateProgram();
    break;
  case BUFFER_TYPE::SHADER_VERTEX:
    bufferID = glCreateShader(GL_VERTEX_SHADER);
    break;
  case BUFFER_TYPE::SHADER_GEOMETRY:
    bufferID = glCreateShader(GL_GEOMETRY_SHADER);
    break;
  case BUFFER_TYPE::SHADER_FRAGMENT:
    bufferID = glCreateShader(GL_FRAGMENT_SHADER);
    break;
  default:
    assert(false);
  }
}

Buffer::~Buffer() {
  switch (type) {
  case BUFFER_TYPE::VAO:
    glDeleteVertexArrays(1, &bufferID);
    break;
  case BUFFER_TYPE::VBO:
  case BUFFER_TYPE::EBO:
    glDeleteBuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::TEXTURE:
    glDeleteTextures(1, &bufferID);
    break;
  case BUFFER_TYPE::FBO:
    glDeleteFramebuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::RBO:
    glDeleteRenderbuffers(1, &bufferID);
    break;
  case BUFFER_TYPE::SHADER_PROGRAM:
    glDeleteProgram(bufferID);
    break;
  case BUFFER_TYPE::SHADER_VERTEX:
  case BUFFER_TYPE::SHADER_GEOMETRY:
  case BUFFER_TYPE::SHADER_FRAGMENT:
    glDeleteShader(bufferID);
    break;
  default:
    assert(false);
  }
}

void Buffer::bind() const {
  switch (type) {
  case BUFFER_TYPE::VAO:
    glBindVertexArray(bufferID);
    break;
  case BUFFER_TYPE::VBO:
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    break;
  case BUFFER_TYPE::EBO:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    break;
  case BUFFER_TYPE::TEXTURE:
    glBindTexture(GL_TEXTURE_2D, bufferID);
    break;
  case BUFFER_TYPE::FBO:
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
    break;
  case BUFFER_TYPE::RBO:
    glBindRenderbuffer(GL_RENDERBUFFER, bufferID);
    break;
  case BUFFER_TYPE::SHADER_PROGRAM:
    glUseProgram(bufferID);
    break;
  case BUFFER_TYPE::SHADER_VERTEX:
  case BUFFER_TYPE::SHADER_GEOMETRY:
  case BUFFER_TYPE::SHADER_FRAGMENT:
    // Not bindable
    break;
  default:
    assert(false);
  }
}