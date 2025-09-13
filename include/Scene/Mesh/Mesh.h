#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "Scene/Mesh/MeshData.h"
#include "Material/Texture.h"
#include "ShaderClass/Shader.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Material/Material.h"


class Mesh
{
public:
    // Constructor
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Material> material);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    // Allow Moving
    // This defines how to efficiently transfer a Mesh's data.
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Destructor
    ~Mesh();

    // Renders the mesh
    void Draw(DrawProperties& globalProperties) const;
    void DrawMeshDepthPrepass(DrawProperties& properties);
    
    AABB CreateAABB(std::vector<Vertex>& vertices);

    unsigned int GetVAO() const { return m_VAO; }
    AABB GetAABB() { return m_AABB; }

    void SetLocation(const glm::vec3& location);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);

    glm::mat4 GetModelMatrix();

    const uint32_t GetFeatureFlag() const;
    void SetShader(std::string vert, std::string frag);

protected:
    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_EBO; // Element Buffer Object

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::shared_ptr<Material>m_Material;

    AABB m_AABB;

    Transform m_Transform;
    std::shared_ptr<Shader> m_Shader;

    DrawProperties m_MeshProperties;
    float m_SpecularPower;
};
