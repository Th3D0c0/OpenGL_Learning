#include "Mesh.h"
#include <vector>
#include <iostream>
#include <GL/glew.h>


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures)
    : m_Vertices(vertices), m_Indices(indices), m_Textures(std::move(textures))
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
}

Mesh::~Mesh()
{
    // De-allocate all resources once the object is destroyed
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::Draw(Shader& shader)
{
    // Counters for texture types to build the uniform names
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        std::string number;
        std::string type = m_Textures[i].getType();

        if (type == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (type == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }

        // Set the sampler uniform in the shader (e.g., "texture_diffuse1")
        shader.setUniformValue((type + number), (int)i);

        // Bind the texture to the correct texture unit
        m_Textures[i].bind(i);
    }

    // Draw the mesh
    glBindVertexArray(m_VAO);   
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw(Shader& shader, unsigned int instanceCount)
{
    // Counters for texture types to build the uniform names
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        std::string number;
        std::string type = m_Textures[i].getType();

        if (type == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (type == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }

        // Set the sampler uniform in the shader (e.g., "texture_diffuse1")
        shader.setUniformValue((type + number), (int)i);

        // Bind the texture to the correct texture unit
        m_Textures[i].bind(i);
    }


    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}