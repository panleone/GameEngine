#include "EntityManager.h"

void EntityManager::update(float deltaTime) {
  iterEntities([&](Entity *e) { e->update(deltaTime); }, true);
}

void EntityManager::render(const Camera &camera) {
  renderLights(camera);
  renderEntities(camera);
}

void EntityManager::renderLights(const Camera &camera) {
  lightShader.use();
  Mat4f pvMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
  for (const PointLight *light : lights) {
    lightShader.setLightColor(light->lightColor);
    lightShader.setPvmMatrix(pvMatrix * light->modelMatrix());
    light->render(lightShader);
  }
}

void EntityManager::renderEntities(const Camera &camera) {
  entityShader.use();
  entityShader.setCamera(camera);
  // Step 1 - Draw all solid Entities
  for (const Entity *entity : solidEntities) {
    renderEntity(entity);
  }
  // Step 2 - Sort transparent entities based on their distance from the camera
  std::map<float, const Entity *> sortedEntities;
  for (const Entity *entity : transparentEntities) {
    float d = mat::distance2(entity->position, camera.getCameraPos());
    sortedEntities.insert(std::pair(d, entity));
  }
  // Step 3 - Display transparent entities from furthest to closest
  for (auto it = sortedEntities.rbegin(); it != sortedEntities.rend(); it++) {
    renderEntity(it->second);
  }
  // TODO: implement an order independent algorithm
  // https://en.wikipedia.org/wiki/Order-independent_transparency
}

void EntityManager::renderEntity(const Entity *entity) {
  // Update light positions in the entity shader
  entityShader.setModelMatrix(entity->modelMatrix());
  size_t found = 0;
  // Find which light are close enough to have an effect
  if (dirLight) {
    entityShader.setDirectionalLight(*dirLight, found);
    found += 1;
  }
  for (PointLight *light : this->lights) {
    if (mat::distance2(light->position, entity->position) < LIGHT_D2_CUTOFF) {
      entityShader.setPointLight(*light, found);
      found += 1;
    }
  }
  entityShader.setNumberOfLights(found);
  entity->render(entityShader);
}

void EntityManager::addPointLight(PointLight *source) {
  lights.push_back(source);
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
  for (Entity *light : lights) {
    fn(light);
  }
}