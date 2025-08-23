#pragma once

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TextCoord;
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};