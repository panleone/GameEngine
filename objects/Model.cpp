#include "Model.h"

#include <glad/glad.h>

// TODO: Might need refactor once there are more complex Models
Model::Model(std::span<float> vertices, std::span<unsigned int> indices,
             std::function<void()> attrPointers)
    : nVertices{indices.size()} {
  // Generate buffers
  glGenVertexArrays(1, &vaoID);
  glGenBuffers(1, &vboID);
  glGenBuffers(1, &eboID);

  // Bind and fill
  glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  // Connect VAO to shader entries
  attrPointers();

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::bindAndDraw() const {
  for (const auto &[i, texture] : textures) {
    texture.bind(i);
  }
  glBindVertexArray(vaoID);
  glDrawElements(GL_TRIANGLES, nVertices, GL_UNSIGNED_INT, 0);
}

void Model::addTexture(unsigned int textureSlot, std::string_view texturePath) {
  textures.push_back(std::make_pair(textureSlot, Texture(texturePath)));
}

Model::~Model() {
  glDeleteBuffers(1, &vboID);
  glDeleteBuffers(1, &eboID);
  glDeleteBuffers(1, &vaoID);
}