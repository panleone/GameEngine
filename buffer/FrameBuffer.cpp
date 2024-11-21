#include "FrameBuffer.h"

#include <glad/glad.h>
#include <stdexcept>

FrameBuffer::FrameBuffer(float bufferWidth, float bufferHeight) {
  rawBuffer = std::make_unique<RawFrameBuffer>();
  rawBuffer->fbo.bind();

  rawBuffer->color.bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         rawBuffer->color.getID(), 0);

  rawBuffer->depth_stencil.bind();
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth,
                        bufferHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rawBuffer->depth_stencil.getID());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error("Frame Buffer is not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}