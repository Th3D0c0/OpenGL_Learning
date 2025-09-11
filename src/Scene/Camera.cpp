#include "Window.h"
#include "Scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

bool Camera::isFocused {false};

// Update the constructor to initialize mouse state
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    m_lastX(0.0f),
    m_lastY(0.0f), 
    m_firstMouse(true)
{
    scroll = 1.0f;
    Position = position;
    m_Transform.SetLocation(position);
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()  
{
    return glm::lookAt(m_Transform.GetLocation(), m_Transform.GetLocation() + Front, Up);
}

void Camera::ProcessInput(GLFWwindow* window, double deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        ProcessKeyboard(UP, deltaTime);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    if (isFocused != true) return;

    float velocity = MovementSpeed * scroll * deltaTime;
    if (direction == FORWARD)
        m_Transform.SetLocation(m_Transform.GetLocation() + Front * velocity);
    if (direction == BACKWARD)
        m_Transform.SetLocation(m_Transform.GetLocation() - Front * velocity);
    if (direction == LEFT)
        m_Transform.SetLocation(m_Transform.GetLocation() - Right * velocity);
    if (direction == RIGHT)
        m_Transform.SetLocation(m_Transform.GetLocation() + Right * velocity);
    if (direction == DOWN)
        m_Transform.SetLocation(m_Transform.GetLocation() - WorldUp * velocity);
    if (direction == UP)
        m_Transform.SetLocation(m_Transform.GetLocation() + WorldUp * velocity);
}

// Update ProcessMouseMovement to handle the first-mouse logic
void Camera::ProcessMouseMovement(float xpos, float ypos, bool constrainPitch)
{
    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top

    m_lastX = xpos;
    m_lastY = ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::ProcessMouseDelta(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += -yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

// Implement the static callback function
void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{

    // 1. Retrieve the camera object stored in the window
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera)
    {
        // 2. Call the non-static member function to process the input
        camera->ProcessMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void Camera::setScrollCallback(GLFWwindow* window)
{
    glfwSetScrollCallback(window, scroll_callback);
}

void Camera::UpdateProjectionMatrix(glm::mat4 newProjectionMat4)
{
    m_ProjectionMatrix = newProjectionMat4;
}

void Camera::UpdateProjectionMatrix(unsigned int width, unsigned int height)
{
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000000.0f);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return m_ProjectionMatrix;
}

glm::mat4 Camera::GetModelMatrix()
{
    return m_Transform.GetModelMatrix();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

// In Camera.cpp
void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (context && context->camera)
    {
        if (yoffset > 0.0)
        {
            context->camera->scroll += 0.3f; // Multiply speed
            if (context->camera->scroll > 50.0f)
            {
                context->camera->scroll = 50.0f;
            }
        }
        if (yoffset < 0.0)
        {
            context->camera->scroll -= 0.3f; // Divide speed
            if (context->camera->scroll < 0.1f) // Prevent speed from getting too slow
            {
                context->camera->scroll = 0.1f;
            }
        }
    }
}

void Camera::SetLocation(const glm::vec3& location)
{
    m_Transform.SetLocation(location);
}

void Camera::SetRotation(const glm::vec3& rotation)
{
    m_Transform.SetRotation(rotation);
}

void Camera::SetScale(const glm::vec3& scale)
{
    m_Transform.SetScale(scale);
}

glm::vec3 Camera::GetLocation()
{
    return m_Transform.GetLocation();
}
