#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <ranges>

static TextureType assimpConverter(aiTextureType type) {
  switch (type) {
  case aiTextureType_SPECULAR:
    return TextureType::SPECULAR;
  case aiTextureType_DIFFUSE:
    return TextureType::DIFFUSE;
  default:
    throw std::runtime_error("assimp texture not supported yet");
  }
}

Mesh::Mesh(std::span<Vertex> vertices, std::span<unsigned int> indices)
    : nVertices{indices.size()} {
  setupMesh(vertices, indices);
}

void Mesh::setupMesh(std::span<Vertex> vertices,
                     std::span<unsigned int> indices) {
  // Generate buffers
  rawMesh = std::make_shared<RawMesh>();

  rawMesh->vao.bind();
  rawMesh->vbo.bind();
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  rawMesh->ebo.bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, position)));
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, normal)));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, texCoords)));

  // Unbind
  glBindVertexArray(0);
}

void Mesh::render(const ShaderProgram &shader) const {
  rawMesh->vao.bind();
  glDrawElements(GL_TRIANGLES, this->nVertices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Model::Model(std::string_view path) { loadModel(path); }

void Model::render(const ShaderProgram &shader) const {
  for (const auto &[mesh_block, textures] : meshes) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for (const auto &[i, texture] : std::views::enumerate(textures)) {
      int number;
      glActiveTexture(GL_TEXTURE0 + i);
      if (texture->getType() == TextureType::DIFFUSE) {
        number = ++diffuseNr;
      } else if (texture->getType() == TextureType::SPECULAR) {
        number = ++specularNr;
      } else {
        throw std::runtime_error("Texture not supported yet");
      }
      // Ignore the return type since some shaders
      // simply might not be using all the textures of the model.
      shader.setTexture(texture->getType(), number, i);
      texture->bind();
    }
    for (const auto &mesh : mesh_block) {
      mesh.render(shader);
    }
  }
}

void Model::loadModel(std::string_view path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw std::runtime_error(
        std::format("Assimp error {}", importer.GetErrorString()));
  }
  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
  loadedTextures.clear();
  directory.clear();
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (int i = 0; i < node->mNumMeshes; i++) {
    Mesh mesh = processMesh(scene->mMeshes[node->mMeshes[i]], scene);
    MeshTextures meshTextures =
        processMeshTexture(scene->mMeshes[node->mMeshes[i]], scene);
    addMesh(std::move(mesh), std::move(meshTextures));
  }
  for (int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.position[0] = mesh->mVertices[i].x;
    vertex.position[1] = mesh->mVertices[i].y;
    vertex.position[2] = mesh->mVertices[i].z;

    if (mesh->mNormals) {
      vertex.normal[0] = mesh->mNormals[i].x;
      vertex.normal[1] = mesh->mNormals[i].y;
      vertex.normal[2] = mesh->mNormals[i].z;
    }

    if (mesh->mTextureCoords[0]) {
      vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
      vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
    }
    vertices.push_back(std::move(vertex));
  }
  for (int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }
  return Mesh{vertices, indices};
}

Model::MeshTextures Model::processMeshTexture(aiMesh *mesh,
                                              const aiScene *scene) {
  MeshTextures textures;
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    MeshTextures diffuseMaps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(),
                    std::make_move_iterator(diffuseMaps.begin()),
                    std::make_move_iterator(diffuseMaps.end()));
    MeshTextures specularMaps =
        loadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(),
                    std::make_move_iterator(specularMaps.begin()),
                    std::make_move_iterator(specularMaps.end()));
  }
  return textures;
}

Model::MeshTextures Model::loadMaterialTextures(aiMaterial *mat,
                                                aiTextureType type) {
  MeshTextures res;
  for (int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string path = std::format("{}/{}", directory, str.C_Str());
    if (loadedTextures.count(path) > 0) {
      res.push_back(loadedTextures.at(path));
      continue;
    }

    std::shared_ptr<Texture> texture =
        std::make_shared<Texture>(path, assimpConverter(type));
    loadedTextures.insert(std::pair(path, texture));
    res.push_back(std::move(texture));
  }
  return res;
}

void Model::addMesh(Mesh mesh, MeshTextures meshTextures) {
  // Check if the same vector of textures has already been loaded
  for (auto &[mesh_block, textures] : meshes) {
    // TODO: consider the case in which textures are the same
    //  but in a different order?
    if (textures == meshTextures) {
      mesh_block.push_back(std::move(mesh));
      return;
    }
  }

  // Create a new mesh block
  meshes.push_back(
      std::pair(std::vector<Mesh>{std::move(mesh)}, std::move(meshTextures)));
}