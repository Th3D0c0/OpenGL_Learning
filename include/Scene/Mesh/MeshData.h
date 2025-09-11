#pragma once

#include "glm/glm.hpp"
#include "ShaderClass/Shader.h"

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

struct DrawProperties
{
    Shader* shader;
    bool isWireframe = false;
    bool useTexture = false;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 ObjectModel;
    float screenWidth;
    float screenHeight;
    unsigned int instanceCount = 0;
    glm::vec3 viewPosition;
};