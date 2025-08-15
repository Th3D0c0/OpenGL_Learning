#include "Mesh.h"
#include <vector>
#include <iostream>
#include <GL/glew.h>


Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
    m_IndexCount = indices.size();

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Copy vertex data into the VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Bind Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    std::cout << indices.size();
    // Set the vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
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

void Mesh::draw()
{
    // Bind the VAO and draw the mesh
    glBindVertexArray(m_VAO);
    //glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);

    // Unbind the VAO for good practice
    glBindVertexArray(0);
}