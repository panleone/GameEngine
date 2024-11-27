#ifndef POST_PROCESSING_SHADER_C
#define POST_PROCESSING_SHADER_C

#include "../Shader.h"
#include "../Uniform.h"

// Binding class with post_processing shader files
class PostProcessingShader : public ShaderProgram {
public:
  PostProcessingShader()
      : ShaderProgram("shaders/post_processing/post_processing.vs",
                      "shaders/post_processing/post_processing.fs") {};

  bool setTexture(TextureType textureType, int textureNumber,
                  int textureUnit) override {
    return false;
  };
  void setPostProcessing(int textureUnit) {
    postProcessing.setUniform(textureUnit);
  }

private:
  Uniform<int> postProcessing{rawProgram->getID(), "postProcessing"};
};

#endif // POST_PROCESSING_SHADER_C