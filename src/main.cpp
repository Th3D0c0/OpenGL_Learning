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
Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
double deltaTime = 0.0f;
double lastFrame = 0.0f;
int frameCount = 0;
double timeSinceLastTitleUpdate = 0.0;

// Physics
double accumulator = 0.0;
const double fixedDeltaTime = 1.0 / 60.0; 

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
    camera.setScrollCallback(window.getNativeWindow());

    // --- Shaders and Objects ---
    Shader lightingShader("Shaders/phongLighting.vert", "Shaders/phongLighting.frag");
    Shader lightSourceShader("Shaders/LightSource.vert", "Shaders/LightSource.frag");
    Shader particleShader("Shaders/ParticleShaders/particle.vert", "Shaders/ParticleShaders/particle.frag");

    // Framebuffer for Imgui
    Framebuffer sceneFramebuffer(1800, 1200);

    // Create Scene
    Scene scene(sceneFramebuffer.GetFramebufferWidth(), sceneFramebuffer.GetFramebufferHeight());

    // Fill the Context Struct for the framebuffer resize callback
    AppContext context;
    context.camera = &camera;
    context.scene = &scene;
    glfwSetWindowUserPointer(nativeWindow, &context);

    DrawProperties depthPrepassDrawProperties;

    Sphere lightSphere(0.1f, 36, 18);
    glm::vec3 lightPos(15.0f, 0.0f, 0.0f);

    Sphere BoundarySphere(8.0f, 36, 18);
    glm::vec3 BoundarySpherePos(0.0f, 0.0f, 0.0f);

    Sphere instancedSphere(1.0f, 16, 12);
    ParticleSystem spherePS(instancedSphere, 5000);

    auto planet = std::make_unique<Planet>();
    planet->LoadMesh(10, 128);
    scene.AddObject(std::move(planet));

    auto light = std::make_unique<Light>();

    Skybox skybox;
    std::vector<std::string> faces {
        "res/SkyTexture/px.png",
        "res/SkyTexture/nx.png",
        "res/SkyTexture/py.png",
        "res/SkyTexture/ny.png",
        "res/SkyTexture/pz.png",
        "res/SkyTexture/nz.png"
    };
    skybox.load(faces);

    // --- MAIN LOOP ---
    while (!window.shouldClose())
    {
        window.pollEvents();
        double currentFrame = glfwGetTime();
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

        // Matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), sceneFramebuffer.GetFramebufferWidth() / sceneFramebuffer.GetFramebufferHeight(), 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        depthPrepassDrawProperties.projection = projection;
        depthPrepassDrawProperties.view = view;
        depthPrepassDrawProperties.screenWidth = sceneFramebuffer.GetFramebufferWidth();
        depthPrepassDrawProperties.screenHeight = sceneFramebuffer.GetFramebufferHeight();

        // Calculate the needed Depth buffer and Light Culling Information BEFORE the sceneFramebuffer
        scene.RenderPrepass(depthPrepassDrawProperties);

        // --- RENDER 3D SCENE TO FRAMEBUFFER ---
        sceneFramebuffer.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);


        // ---------Start Drawing Objects---------
        glm::vec3 lightWorldPosition = glm::vec3(15.0f, 5.0f, 20.0f); // Put light in the world
        auto frameLight = std::make_unique<Light>();
        frameLight->positionVS = view * glm::vec4(lightWorldPosition, 1.0f); // Transform to View Space
        frameLight->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        frameLight->intensity = 15.0f; // Increase intensity significantly
        frameLight->range = 100.0f;
        frameLight->type = 1; // Point light
        frameLight->enabled = 1;
        scene.AddLight(std::move(frameLight));
        scene.UpdateLightBuffer();

        scene.RenderScene(view, projection);

        skybox.draw(view, projection);

		
        //BoundarySphere.Draw(lightingShader, true, false);

        // Physics start
        accumulator += deltaTime;   
        //// Particles
        //while (accumulator >= fixedDeltaTime)
        //{
        //    spherePS.Update(fixedDeltaTime);
        //    accumulator -= fixedDeltaTime;
        //}
        //    spherePS.Draw(particleShader, view, projection, lightPos, camera);


        //------------End Drawing Objects------------

        sceneFramebuffer.unbind();

        window.startImGUIFrame();
        ImGui::Begin("Count");
        ImGui::Value("Value: %d", spherePS.GetParticleCount());
        ImGui::End();
        window.DrawSceneView(sceneFramebuffer, camera, window.getNativeWindow());
        window.DrawImGUIControlsWindow(lightPos);

        window.ImGUIRender();
        window.swapBuffers();
    }

    // --- CLEANUP ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
