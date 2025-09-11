#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
    Transform();

    void SetScale(const glm::vec3& newScale);
    void SetLocation(const glm::vec3& newLocation);
    void SetRotation(const glm::vec3 rotation);


    glm::vec3 GetScale();
    glm::vec3 GetLocation();
    glm::vec3 GetRotation();

    glm::mat4 GetModelMatrix() const;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Rotation; // Stored as degrees
    glm::vec3 m_Scale;
};