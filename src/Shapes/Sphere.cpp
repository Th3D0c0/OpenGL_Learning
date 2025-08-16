#include <Texture.h>
#include "Sphere.h"
#include <vector>
#include <cmath>


// Keep these includes for M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Sphere::Sphere(float radius, int sectorCount, int stackCount)
	:Mesh(
		[&]() {
            std::vector<Vertex> vertices;
            const float PI = M_PI;
            Vertex vertex;
            for (int i = 0; i <= stackCount; ++i)
            {
                float stackAngle = PI / 2 - i * PI / stackCount;
                float xy = radius * cosf(stackAngle);
                float z = radius * sinf(stackAngle);
                for (int j = 0; j <= sectorCount; ++j)
                {
                    float sectorAngle = j * 2 * PI / sectorCount;
                    float x = xy * cosf(sectorAngle);
                    float y = xy * sinf(sectorAngle);

                    // Set Vertex Position
                    vertex.Position = glm::vec3(x, y, z);

                    // Set vertex Normal
                    vertex.Normal = glm::vec3(x / radius, y / radius, z / radius);

                    // Set Vertex Texture
                    vertex.TextCoord.x = (float)j / sectorCount;
                    vertex.TextCoord.y = (float)i / stackCount;

                    vertices.push_back(vertex);
                }
            }
            return vertices;
		}(),
        [&]() 
        {
            std::vector<unsigned int> indices;
            for (int i = 0; i < stackCount; ++i)
            {
                int k1 = i * (sectorCount + 1);
                int k2 = k1 + sectorCount + 1;
                for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                {
                    if (i != 0)
                    {
                        indices.push_back(k1);
                        indices.push_back(k2);
                        indices.push_back(k1 + 1);
                    }
                    if (i != (stackCount - 1))
                    {
                        indices.push_back(k1 + 1);
                        indices.push_back(k2);
                        indices.push_back(k2 + 1);
                    }
                }
            }
            return indices;
        }(),
            std::vector<Texture>()
	)
{
}