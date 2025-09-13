#include "Scene/Mesh/Mesh.h"
#include <vector>
#include <iostream>
#include <GL/glew.h>

#include "../../out/build/x64-Debug/_deps/assimp-src/contrib/openddlparser/include/openddlparser/OpenDDLCommon.h"


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Material> material)
    : m_Vertices(vertices), m_Indices(indices), m_SpecularPower(8.0f), m_Material(material)
{
    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Copy vertex data into the VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Bind Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, Position));
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, Normal));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, TextCoord));
    glEnableVertexAttribArray(2);

    // Info: Particle System takes the next 3 Attribute locations

    // Unbind the VAO to prevent accidental modification
    glBindVertexArray(0);

    m_AABB = CreateAABB(m_Vertices);
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_Vertices(std::move(other.m_Vertices)),
    m_Indices(std::move(other.m_Indices)),
    m_Material(std::move(other.m_Material)),
    m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO) // Transfer handles
{
    // IMPORTANT: Null out the other object's handles so its
    // destructor doesn't release our GPU resources.
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

// Move assignment operator (similar logic)
Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        // Release existing resources...
        // (call your glDeleteBuffers, etc. here)

// Release this object's own resources first
        if (m_VAO != 0)
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);
        }

        // Now, transfer resources from the other object
        m_Vertices = std::move(other.m_Vertices);
        m_Indices = std::move(other.m_Indices);
        m_Material = std::move(other.m_Material);
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;

        // And null out the source object
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    // De-allocate all resources once the object is destroyed
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
}

void Mesh::Draw(DrawProperties& globalProperties) const
{
    // Counters for texture types to build the uniform names
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    std::vector<const Texture*> textures = m_Material->GetAllTextures();

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string number;
        std::string type = textures[i]->getType();

        if (type == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (type == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }
        else if (type == "texture_normal")
        {
            number = std::to_string(specularNr++);
        }

        // Set the sampler uniform in the shader (e.g., "texture_diffuse1")
        globalProperties.shader->setUniformValue((type + number), (int)i);

        // Bind the texture to the correct texture unit
        textures[i]->bind(i);
    }

    globalProperties.shader->setUniformValue("model", m_Transform.GetModelMatrix());
    globalProperties.shader->setUniformValue("specularPower", m_SpecularPower);

    if (globalProperties.instanceCount > 0)
    {
        if (globalProperties.isWireframe)
        {
            glBindVertexArray(m_VAO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, globalProperties.instanceCount);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }
        else
        {
	    glBindVertexArray(m_VAO);
	    glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, globalProperties.instanceCount);
	    glBindVertexArray(0);
	    glActiveTexture(GL_TEXTURE0);
        }
    }
    else
    {
        if (globalProperties.isWireframe)
        {
            glBindVertexArray(m_VAO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }
        else
        {
            glBindVertexArray(m_VAO);
            glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }
    }
    // Always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawMeshDepthPrepass(DrawProperties& properties)
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

AABB Mesh::CreateAABB(std::vector<Vertex>& vertices)
{
    AABB result;
    if (vertices.size() <= 0) return result;

    result.min = vertices[0].Position;
    result.max = vertices[0].Position;

    for (int i = 1; i < vertices.size(); i++)
    {
        result.min.x = glm::min(result.min.x, vertices[i].Position.x);
        result.min.y = glm::min(result.min.y, vertices[i].Position.y);
        result.min.z = glm::min(result.min.z, vertices[i].Position.z);

        result.max.x = glm::max(result.max.x, vertices[i].Position.x);
        result.max.y = glm::max(result.max.y, vertices[i].Position.y);
        result.max.z = glm::max(result.max.z, vertices[i].Position.z);
    }
    return result;
}

void Mesh::SetLocation(const glm::vec3& location)
{
    m_Transform.SetLocation(location);
}

void Mesh::SetRotation(const glm::vec3& rotation)
{
    m_Transform.SetRotation(rotation);
}

void Mesh::SetScale(const glm::vec3& scale)
{
    m_Transform.SetScale(scale);
}

glm::mat4 Mesh::GetModelMatrix()
{
    return m_Transform.GetModelMatrix();
}

const uint32_t Mesh::GetFeatureFlag() const
{
    return m_Material->GetFeatureFlag();
}

void Mesh::SetShader(std::string vert, std::string frag)
{
    m_Shader = std::make_unique<Shader>(vert.c_str(), frag.c_str());
}
