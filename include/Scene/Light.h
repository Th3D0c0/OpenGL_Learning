#pragma once

#include <glm/glm.hpp>

struct Light
{
    glm::vec4 positionVS;   // Position in View Space (most useful for culling)
    glm::vec4 color;
    glm::vec4 directionVS;  // Direction in View Space

    float spotlightAngle;
    float range;
    float intensity;
    int type;               // 0: Directional, 1: Point, 2: Spot
    int enabled;            // Use int instead of bool (1 for true, 0 for false)
    // Add explicit padding to ensure correct alignment
    float _padding[3];
};