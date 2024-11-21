#ifndef FRAME_BUFFER_C
#define FRAME_BUFFER_C

#include <memory>

#include "Buffer.h"

// Color buffer + depth buffer + stencil buffer
struct RawFrameBuffer {
  Buffer fbo{BUFFER_TYPE::FBO};
  // TODO: This is not the most general frame buffer
  Buffer color{BUFFER_TYPE::TEXTURE};
  Buffer depth_stencil{BUFFER_TYPE::RBO};
};
class FrameBuffer {
private:
  std::unique_ptr<RawFrameBuffer> rawBuffer;

public:
  FrameBuffer(float bufferWidth, float bufferHeight);
  FrameBuffer(FrameBuffer &&buffer) = default;
  FrameBuffer(const FrameBuffer &buffer) = delete;
  void bind() const { rawBuffer->fbo.bind(); }
  void bindColor() const { rawBuffer->color.bind(); }
};

#endif // FRAME_BUFFER_C