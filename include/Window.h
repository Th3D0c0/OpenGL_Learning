#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <array>
#ifdef ENGINE_EDITOR
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif
#include "Framebuffer.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"

// Context struct for framebuffer resize callback
struct AppContext
{
    Camera* camera;
    Scene* scene;
    GLFWwindow* window;
    Framebuffer* sceneFramebuffer;
};

class Window
{
public:
    Window(int width, int height, const char* title, bool fullscreen = false);
    ~Window();

    // Checks if the main loop should continue
    bool shouldClose();

    // Swaps buffers and polls for events
    void swapBuffers();
    void SetupRenderQuad();
    void pollEvents();

    GLFWwindow* getNativeWindow() const;

    // Make this class non-copyable to prevent issues with the window pointer
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void RenderWindow(Shader& renderShader, Framebuffer& framebuffer);

#ifdef ENGINE_EDITOR
    void startImGUIFrame();
    void ImGUIRender();

    // IMGUI WINDOWS
    void DrawImGUIControlsWindow(glm::vec3& lightPos);
    void DrawSceneView(Framebuffer& framebuffer, Camera& camera, GLFWwindow* nativeWindow, AppContext& context);
#endif

private:
    GLFWwindow* m_Window; // Opaque window handle
    bool m_CursorEnabled;

    std::array<float, 24> m_QuadVertices = {
        // positions    // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };

    unsigned int m_QuadVAO, m_QuadVBO;
};
