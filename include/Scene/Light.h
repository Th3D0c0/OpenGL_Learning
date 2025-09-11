#pragma once

#include <glm/glm.hpp>

struct alignas(16) Light
{
    glm::vec4 positionWS;
    glm::vec4 directionWS;
    glm::vec4 color;
    float spotAngle;
    float range;
    float intensity;
    int type;
};

static_assert(sizeof(Light) == 64, "Light struct size/layout mismatch with GLSL!");