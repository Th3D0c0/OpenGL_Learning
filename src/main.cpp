#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Sphere.h"
#include "Framebuffer.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Global Camera and Timing
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void process_app_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // Use direct initialization
    Window window(1800, 1200, "Physics Simulation", false);
    GLFWwindow* nativeWindow = window.getNativeWindow();

    // --- Shaders and Objects ---
    Shader lightingShader("Shaders/phongLighting.vert", "Shaders/phongLighting.frag");
    Shader lightSourceShader("Shaders/LightSource.vert", "Shaders/LightSource.frag");

    std::vector<float> triangleVertices = {
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    std::vector<unsigned int> triangleIndices {0, 1, 2};
    Mesh triangleMesh(triangleVertices, triangleIndices);

    Framebuffer sceneFramebuffer(1800, 1200);
    glm::vec2 sceneViewportSize = {1800, 1200};

    Sphere lightSphere(1.0f, 36, 18);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    glfwSetWindowUserPointer(nativeWindow, &camera);
    glfwSetCursorPosCallback(nativeWindow, Camera::MouseCallback);

    // --- MAIN LOOP ---
    while (!window.shouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_app_input(nativeWindow);
        camera.ProcessInput(nativeWindow, deltaTime);

        // --- RENDER 3D SCENE TO FRAMEBUFFER ---
        sceneFramebuffer.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), sceneViewportSize.x / sceneViewportSize.y, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Draw the lit triangle
        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model);
        triangleMesh.draw();

        // Draw the light source sphere
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model);
        lightSphere.draw();

        sceneFramebuffer.unbind();

        window.startImGUIFrame();
        window.DrawImGUIDockspace(sceneFramebuffer, sceneViewportSize);
        window.DrawImGUIControlsWindow(lightPos);

        window.ImGUIRender();

        window.swapBuffers();
        window.pollEvents();
    }

    // --- CLEANUP ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
