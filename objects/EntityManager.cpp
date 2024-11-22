#include "EntityManager.h"

void EntityManager::update(float deltaTime) {
  iterEntities([&](Entity *e) { e->update(deltaTime); }, true);
  // Update light positions in the entity shader
  auto &entityShader = shaders.at("entity");
  entityShader.use();
  for (const auto &[i, light] : std::views::enumerate(lights)) {
    std::string uniformName = std::format("lights[{}]", i);
    entityShader.setUniform(std::format("{}.position", uniformName),
                            light->position);
  }
}

void EntityManager::render(const Camera &camera) {
  renderLights(camera);
  renderEntities(camera);
}

void EntityManager::renderLights(const Camera &camera) {
  auto &lightShader = shaders.at("light");
  lightShader.use();
  for (const PointLight *light : lights) {
    lightShader.setUniform("lightColor", light->lightColor);
    lightShader.setUniform("pvmMatrix", camera.getProjectionMatrix() *
                                            camera.getViewMatrix() *
                                            light->modelMatrix());
    light->render(lightShader);
  }
}

void EntityManager::renderEntities(const Camera &camera) {
  auto &entityShader = shaders.at("entity");
  entityShader.use();
  auto pvMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
  entityShader.setUniform("pvMatrix", pvMatrix);
  entityShader.setUniform("eyePos", camera.getCameraPos());
  // Step 1 - Draw all solid Entities
  for (const Entity *entity : solidEntities) {
    entityShader.setUniform("mMatrix", entity->modelMatrix());
    entity->render(entityShader);
  }
  // Step 2 - Sort transparent entities based on their distance from the camera
  std::map<float, const Entity *> sortedEntities;
  for (const Entity *entity : transparentEntities) {
    float d = mat::distance2(entity->position, camera.getCameraPos());
    sortedEntities.insert(std::pair(d, entity));
  }
  // Step 3 - Display transparent entities from furthest to closest
  for (auto it = sortedEntities.rbegin(); it != sortedEntities.rend(); it++) {
    entityShader.setUniform("mMatrix", it->second->modelMatrix());
    it->second->render(entityShader);
  }
  // TODO: implement an order independent algorithm
  // https://en.wikipedia.org/wiki/Order-independent_transparency
}

void EntityManager::addPointLight(PointLight *source) {
  lights.push_back(source);
  auto &entityShader = shaders.at("entity");
  entityShader.use();

  std::string uniformName = std::format("lights[{}]", lights.size() - 1);
  entityShader.setUniform(std::format("{}.ambient", uniformName),
                          source->ambientIntensity);
  entityShader.setUniform(std::format("{}.diffuse", uniformName),
                          source->diffuseIntensity);
  entityShader.setUniform(std::format("{}.specular", uniformName),
                          source->specularIntensity);
  entityShader.setUniform(std::format("{}.attenuation", uniformName),
                          source->attenuationCoefficients);
  entityShader.setUniform("nLights", lights.size());
}

void EntityManager::setDirectionalLight(DirectionalLight *source) {
    dirLight = source;
}

void EntityManager::iterEntities(std::function<void(Entity *)> fn,
                                 bool includeLights) {
  for (Entity *entity : solidEntities) {
    fn(entity);
  }
  for (Entity *entity : transparentEntities) {
    fn(entity);
  }
  if (!includeLights)
    return;
  for (Entity *light : transparentEntities) {
    fn(light);
  }
}