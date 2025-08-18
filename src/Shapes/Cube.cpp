#include "Shapes/Cube.h"
#include <vector>

Cube::Cube(float length, float height, float width)
    : Mesh(
        // Lambda to generate vertices
        [&]() {
            float l = length / 2.0f;
            float h = height / 2.0f;
            float w = width / 2.0f;

            std::vector<Vertex> vertices = {
                // Front face
                { { -l, -h,  w }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
                { {  l, -h,  w }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
                { {  l,  h,  w }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
                { { -l,  h,  w }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
                                                                           
                // Back face                                               
                { {  l, -h, -w }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } }, 
                { { -l, -h, -w }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } }, 
                { { -l,  h, -w }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } }, 
                { {  l,  h, -w }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } }, 
                                                                           
                // Left face                                               
                { { -l, -h, -w }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }, 
                { { -l, -h,  w }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } }, 
                { { -l,  h,  w }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } }, 
                { { -l,  h, -w }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } }, 

                // Right face
                { {  l, -h,  w }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
                { {  l, -h, -w }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
                { {  l,  h, -w }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
                { {  l,  h,  w }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },

                // Top face
                { { -l,  h,  w }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
                { {  l,  h,  w }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
                { {  l,  h, -w }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
                { { -l,  h, -w }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },

                // Bottom face
                { { -l, -h, -w }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
                { {  l, -h, -w }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
                { {  l, -h,  w }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
                { { -l, -h,  w }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }
            };
            return vertices;
        }(),
            // Lambda to generate indices
            [&]() {
            std::vector<unsigned int> indices;
            for (unsigned int i = 0; i < 6; ++i)
            {
                indices.push_back(i * 4 + 0);
                indices.push_back(i * 4 + 1);
                indices.push_back(i * 4 + 2);
                indices.push_back(i * 4 + 0);
                indices.push_back(i * 4 + 2);
                indices.push_back(i * 4 + 3);
            }
            return indices;
            }(),
                // Empty texture vector
                std::vector<Texture>()
                )
{
}

Cube::~Cube()
{
}