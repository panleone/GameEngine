#include "EntityShader.h"

#include <cassert>

UniformLight &EntityShader::setLight(const Light &light, int lightNumber) {
  if (lightNumber + 1 > uniformLights.size()) {
    assert(lightNumber == uniformLights.size());
    uniformLights.push_back(UniformLight{
        rawProgram->getID(), std::format("lights[{}]", lightNumber)});
  }
  auto &uLight = uniformLights.at(lightNumber);
  uLight.ambient.setUniform(light.ambientIntensity);
  uLight.diffuse.setUniform(light.diffuseIntensity);
  uLight.specular.setUniform(light.specularIntensity);
  uLight.lightVector.setUniform(light.getLightVector());
  return uLight;
}

void EntityShader::setPointLight(const PointLight &light, int lightNumber) {
  auto &uLight = setLight(light, lightNumber);
  uLight.attenuation.setUniform(light.attenuationCoefficients);
}

void EntityShader::setDirectionalLight(const DirectionalLight &light,
                                       int lightNumber) {
  setLight(light, lightNumber);
}

void EntityShader::setCamera(const Camera &camera) {
  cameraPV.setUniform(camera.getProjectionMatrix() * camera.getViewMatrix());
  cameraPos.setUniform(camera.getCameraPos());
}

void EntityShader::setNumberOfLights(int n) { nLights.setUniform(n); }

void EntityShader::setModelMatrix(const Mat4f &m) { modelMatrix.setUniform(m); }

bool EntityShader::setTexture(TextureType textureType, int textureNumber,
                              int textureUnit) {
  if (textureNumber > 1) {
    return false;
  }
  if (textureType == TextureType::SPECULAR) {
    materialSpecular.setUniform(textureUnit);
  } else if (textureType == TextureType::DIFFUSE) {
    materialDiffuse.setUniform(textureUnit);
  } else {
    return false;
  }
  return true;
}