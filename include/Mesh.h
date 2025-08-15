#pragma once
#include <vector>

class Mesh
{
public:
    // Constructor
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    // Destructor
    ~Mesh();

    // Renders the mesh
    void draw();

protected:
    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_EBO; // Element Buffer Object
    unsigned int m_IndexCount; // Indices Count
};
