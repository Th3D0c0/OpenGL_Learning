#pragma once

#include <glm/glm.hpp> 
#include <GLFW/glfw3.h> 

// Forward declare GLFWwindow
struct GLFWwindow;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    DOWN,
    UP
};

// ... default values ...
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float scroll = 1.0f;
    float MovementSpeed;
    float MouseSensitivity;


public:
    // constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    glm::mat4 GetViewMatrix() const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseDelta(float xoffset, float yoffset, bool constrainPitch = true);
    // Processes keyboard input from a GLFW window
    void ProcessInput(GLFWwindow* window, double deltaTime);

    // Static callback function that GLFW will call
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void setScrollCallback(GLFWwindow* window);

    void SetFirstMouse(bool value) { m_firstMouse = value; };

public:
    static bool isFocused;

private:
    // Mouse state now lives inside the camera
    float m_lastX;
    float m_lastY;
    bool m_firstMouse;

    void updateCameraVectors();
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};