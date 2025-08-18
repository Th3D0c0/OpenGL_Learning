#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "MeshData.h"
#include "Texture.h"
#include "Shader.h"

class Mesh
{
public:
    // Constructor
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures);
    // Destructor
    ~Mesh();

    // Renders the mesh
    void Draw(Shader& shader);
    void Draw(Shader& shader, unsigned int instanceCount);
    unsigned int GetVAO() const  { return m_VAO; }

protected:
    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_EBO; // Element Buffer Object

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;
};
