#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "MeshData.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "Camera.h"
#include "Shader.h"

class Mesh
{
public:
    // Constructor
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures);
    // Destructor
    ~Mesh();

    // Renders the mesh
    void Draw(Shader& shader, bool isWireframe, bool useTexture, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, Camera camera);
    void Draw(Shader& shader, unsigned int instanceCount, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, Camera camera);
    
    AABB CreateAABB(std::vector<Vertex>& vertices);

    unsigned int GetVAO() const { return m_VAO; }
    AABB GetAABB() { return m_AABB; }

    void SetLocation(const glm::vec3& location);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);

    void SetShader(std::string vert, std::string frag);

protected:
    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_EBO; // Element Buffer Object

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;

    AABB m_AABB;

    Transform m_Transform;
    std::shared_ptr<Shader> m_Shader;
};
