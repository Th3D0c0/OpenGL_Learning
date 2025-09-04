#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"

#include <vector>
#include <iostream>

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

    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const 
{
    return glm::lookAt(Position, Position + Front, Up);
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

    float velocity = MovementSpeed* scroll * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == DOWN)
        Position -= WorldUp * velocity;
    if (direction == UP)
        Position += WorldUp * velocity;
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

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // You can retrieve your camera object like this if you've set it up
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera)
    {
        if (yoffset > 0.0)
        {
			camera->scroll += 0.5f;
        }
        if (yoffset < 0.0)
        {
        	camera->scroll -= 0.5f;
        }
        printf("%f", xoffset);
    }
}