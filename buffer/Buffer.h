#ifndef BUFFER_C
#define BUFFER_C

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
class Buffer {
private:
  BUFFER_TYPE type;
  unsigned int bufferID;

public:
  Buffer(BUFFER_TYPE type);
  ~Buffer();
  Buffer(const Buffer &buffer) = delete;
  Buffer(Buffer &&buffer) = delete;
  unsigned int getID() const { return bufferID; };
  void bind() const;
};

#endif // BUFFER_C