#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Framebuffer.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"

// Context struct for framebuffer resize callback
struct AppContext
{
    Camera* camera;
    Scene* scene;
};

class Window
{
public:
    Window(int width, int height, const char* title, bool fullscreen = false);
    ~Window();

    // Checks if the main loop should continue
    bool shouldClose();

    void startImGUIFrame();
    void ImGUIRender();

    // Swaps buffers and polls for events
    void swapBuffers();

    void pollEvents();

    GLFWwindow* getNativeWindow() const;

    // Make this class non-copyable to prevent issues with the window pointer
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // IMGUI WINDOWS
    void DrawImGUIControlsWindow(glm::vec3& lightPos);
    void DrawSceneView(Framebuffer& framebuffer, Camera& camera, GLFWwindow* nativeWindow);

private:
    GLFWwindow* m_Window; // Opaque window handle
    bool m_CursorEnabled;
};
