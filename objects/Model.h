#ifndef MODEL_C
#define MODEL_C

#include <assimp/scene.h>
#include <memory>
#include <span>
#include <unordered_map>

#include "../math/Matrix.h"
#include "../shaders/Shader.h"
#include "../textures/Texture.h"

/**
 * VBO + VAO + EBO.
 * Neither copyable nor movable.
 * Wrap it inside a shared_ptr<> to create different Models that share the same
 * VAO
 */
class RawMesh {
public:
  unsigned int vaoID;
  unsigned int vboID;
  unsigned int eboID;
  RawMesh(const RawMesh &rawMesh) = delete;
  RawMesh(RawMesh &&rawMesh) = delete;
  RawMesh() {
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &eboID);
  }
  ~RawMesh() {
    glDeleteBuffers(1, &eboID);
    glDeleteBuffers(1, &vboID);
    glDeleteVertexArrays(1, &vaoID);
  }
  const unsigned int &getVAO() { return vaoID; };
  const unsigned int &getVBO() { return vboID; };
  const unsigned int &getEBO() { return eboID; };
};

struct Vertex {
  // C style arrays to have memory correctly aligned
  float position[3];
  float normal[3];
  float texCoords[2];
};

class Mesh {
public:
  Mesh(std::span<Vertex> vertices, std::span<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures);
  // Thanks to shared_ptr copy is cheap.
  Mesh(const Mesh &mesh) = default;
  Mesh(Mesh &&mesh) = default;
  void render(const ShaderProgram &shader) const;

private:
  std::shared_ptr<RawMesh> rawMesh;
  // TODO: make an owner class of all texture
  //  and use references instead of shared_ptr?
  std::vector<std::shared_ptr<Texture>> textures;
  size_t nVertices;
  void setupMesh(std::span<Vertex> vertices, std::span<unsigned int> indices);
};

class Model {
public:
  /**
   * Build a model from an imported external model, like .obj files.
   */
  explicit Model(std::string_view path);
  // Thanks to shared_ptr copy is cheap.
  Model(const Model &model) = default;
  Model(Model &&model) = default;

  void render(const ShaderProgram &program) const;

private:
  std::vector<Mesh> meshes;
  // The following variables are cleared once the Model is loaded.
  // map path -> texture,
  std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
  std::string directory;

  void loadModel(std::string_view path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<std::shared_ptr<Texture>>
  loadMaterialTextures(aiMaterial *mat, aiTextureType type);
};

#endif // MODEL_C