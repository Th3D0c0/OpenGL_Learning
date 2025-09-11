#include "Scene/Transform.h"

Transform::Transform()
	: m_Position(0.0f), m_Rotation(0.0f), m_Scale(1.0f)
{
}

void Transform::SetScale(const glm::vec3& newScale)
{
    m_Scale = newScale;
}

void Transform::SetLocation(const glm::vec3& newLocation)
{
    m_Position = newLocation;
}

void Transform::SetRotation(const glm::vec3 rotation)
{
    m_Rotation = rotation;
}

glm::vec3 Transform::GetScale()
{
    return m_Scale;
}

glm::vec3 Transform::GetLocation()
{
    return m_Position;
}

glm::vec3 Transform::GetRotation()
{
    return m_Rotation;
}

glm::mat4 Transform::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);

    // Apply translation
    model = glm::translate(model, m_Position);

    // Apply rotation (example for Y-axis rotation)
    model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply the stored scale
    model = glm::scale(model, m_Scale);

    return model;
}
