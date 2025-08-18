#include "Config.h"

#ifdef _WIN32
#include <windows.h>
extern "C" {
    // For NVIDIA GPUs
    __declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
    // For AMD GPUs
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

// Global Camera and Timing
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frameCount = 0;
double timeSinceLastTitleUpdate = 0.0;

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

    //std::vector<Vertex> triangleVertices = {
    //    // Positions                   // Normals                   // Texture Coords
    //    {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
    //    {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
    //    {glm::vec3(0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)}
    //};

    //std::vector<unsigned int> triangleIndices {0, 1, 2};

    //std::vector<Texture> triangleTextures;
    //Texture brickTexture("res/brick.png", "texture_diffuse");
    //brickTexture.LoadTexture();
    //triangleTextures.push_back(std::move(brickTexture));

    //Mesh triangleMesh(triangleVertices, triangleIndices, triangleTextures);

    // Framebuffer for Imgui
    Framebuffer sceneFramebuffer(1800, 1200);
    glm::vec2 sceneViewportSize = {1800, 1200};

    Sphere lightSphere(1.0f, 36, 18);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    Sphere instancedSphere(0.1, 6, 4);
    ParticleSystem spherePS(instancedSphere, 100000);

    glfwSetWindowUserPointer(nativeWindow, &camera);

    // --- MAIN LOOP ---
    while (!window.shouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Ms/FPS viewer
        frameCount++;
        timeSinceLastTitleUpdate += deltaTime;
        if (timeSinceLastTitleUpdate >= 1.0) // If 1 second has passed
        {
            char title[256];
            // Format the title string with stats
            snprintf(title, sizeof(title), "Physics Test | %.2f ms/frame | %d FPS", (1000.0 / frameCount), frameCount);
            glfwSetWindowTitle(nativeWindow, title);

            // Reset counters
            frameCount = 0;
            timeSinceLastTitleUpdate -= 1.0;
        }

        process_app_input(nativeWindow);
        camera.ProcessInput(nativeWindow, deltaTime);


        // --- RENDER 3D SCENE TO FRAMEBUFFER ---
        sceneFramebuffer.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), sceneViewportSize.x / sceneViewportSize.y, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // Draw the lit triangle
        lightingShader.use();
        lightingShader.setUniformValue("lightPos", lightPos);
        lightingShader.setUniformValue("viewPos", camera.Position);
        lightingShader.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setUniformValue("projection", projection);
        lightingShader.setUniformValue("view", view);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        lightingShader.setUniformValue("model", model);

        spherePS.Update(deltaTime);
        instancedSphere.Draw(lightingShader, 10000);
  /*      triangleMesh.Draw(lightingShader);*/

        // Draw the light source sphere
        lightSourceShader.use();
        lightSourceShader.setUniformValue("projection", projection);
        lightSourceShader.setUniformValue("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightSourceShader.setUniformValue("model", model);
        lightSphere.Draw(lightSourceShader);

        sceneFramebuffer.unbind();

        window.startImGUIFrame();
        window.DrawSceneView(sceneFramebuffer, camera, window.getNativeWindow());
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
