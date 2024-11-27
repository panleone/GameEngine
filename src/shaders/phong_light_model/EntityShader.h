#ifndef ENTITY_SHADER_C
#define ENTITY_SHADER_C

#include <vector>

#include "../../Camera.h"
#include "../../objects/Light.h"
#include "../Uniform.h"
#include "../Shader.h"

// Binding class with phong_light shader files
class UniformLight {
public:
  Uniform<Vec4f> lightVector;
  Uniform<Vec3f> ambient;
  Uniform<Vec3f> diffuse;
  Uniform<Vec3f> specular;
  Uniform<Vec3f> attenuation;
  UniformLight(GLuint ID, const std::string &lightName)
      : lightVector{ID, lightName + std::string(".lightVector")},
        ambient{ID, lightName + std::string(".ambient")},
        diffuse{ID, lightName + std::string(".diffuse")},
        specular{ID, lightName + std::string(".specular")},
        attenuation{ID, lightName + std::string(".attenuation")} {};
};

class EntityShader : public ShaderProgram {
private:
  std::vector<UniformLight> uniformLights;
  Uniform<Mat4f> cameraPV{rawProgram->getID(), "pvMatrix"};
  Uniform<Vec3f> cameraPos{rawProgram->getID(), "eyePos"};
  Uniform<Mat4f> modelMatrix{rawProgram->getID(), "mMatrix"};
  Uniform<int> nLights{rawProgram->getID(), "nLights"};
  Uniform<int> materialSpecular{rawProgram->getID(),
                                "material.texture_specular1"};
  Uniform<int> materialDiffuse{rawProgram->getID(),
                               "material.texture_diffuse1"};
  UniformLight &setLight(const Light &light, int lightNumber);

public:
  void setPointLight(const PointLight &light, int lightNumber);
  void setDirectionalLight(const DirectionalLight &light, int lightNumber);
  void setCamera(const Camera &camera);
  void setModelMatrix(const Mat4f &m);
  void setNumberOfLights(int n);
  EntityShader()
      : ShaderProgram("src/shaders/phong_light_model/phong_light.vs",
                      "src/shaders/phong_light_model/phong_light.fs",
                      "src/shaders/phong_light_model/phong_light.gs") {};

  bool setTexture(TextureType textureType, int textureNumber,
                  int textureUnit) override;
};

#endif // ENTITY_SHADER_C