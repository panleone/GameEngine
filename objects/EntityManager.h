#ifndef ENTITY_MANAGER_C
#define ENTITY_MANAGER_C

#include <functional>
#include <map>
#include <ranges>

#include "../Camera.h"
#include "../shaders/Shader.h"
#include "Light.h"
#include "Entity.h"
#include "Model.h"

class EntityManager {
  // Distance after which point lights have no effect
  static constexpr float LIGHT_D2_CUTOFF = 100.0f;
  // vector of pointers due to possible future inheritance
  // TODO: instead use references? unique/shared_ptrs?

  // Solid non-transparent entities
  std::vector<Entity *> solidEntities;
  // Entities that are transparent or partially transparent,
  // for example a window
  std::vector<Entity *> transparentEntities;
  // Point light sources
  std::vector<PointLight *> lights;
  // Directional light, for the moment at most one because
  // they are expensive to simulate (non local, they act on all entities)
  DirectionalLight *dirLight{nullptr};
  std::map<std::string, ShaderProgram> &shaders;

public:
  EntityManager(std::map<std::string, ShaderProgram> &shaders)
      : shaders{shaders} {};
  void addSolidEntity(Entity *entity) { solidEntities.push_back(entity); };
  void addTransparentEntity(Entity *entity) {
    transparentEntities.push_back(entity);
  };
  void addPointLight(PointLight *source);
  void setDirectionalLight(DirectionalLight *source);

  void update(float deltaTime);
  void render(const Camera &camera);

private:
  void iterEntities(std::function<void(Entity *)> fn, bool includeLights);
  void renderLights(const Camera &camera);
  void renderEntities(const Camera &camera);
  void renderEntity(const Entity *entity);
};

#endif // ENTITY_MANAGER_C