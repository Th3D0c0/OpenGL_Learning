#include "Window.h"
#include "Scene/Scene.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update OpenGL's viewport to match the new window size
    glViewport(0, 0, width, height);

    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (context)
    {
        if (width == 0 || height == 0)
        {
            return;
        }
        // Tell your scene to handle the resize logic
        context->scene->OnWindowResize(width, height);
        context->sceneFramebuffer->resize(width, height);
        context->camera->UpdateProjectionMatrix(width, height);
    }
}
void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Window::Window(int width, int height, const char* title, bool fullscreen)
    :m_CursorEnabled {true}
{
    // 1. Initialize GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        // Handle initialization failure
        return;
    }

    // 2. Set window hints for OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = nullptr;
    if (fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
    }

    m_Window = glfwCreateWindow(width, height, title, monitor, NULL);

    // 3. Create the GLFW window
    if (!m_Window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        // Handle window creation failure
        return;
    }
    glfwMakeContextCurrent(m_Window);

    glfwSwapInterval(0);

    if(!m_CursorEnabled)
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Register callback Function
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    // Manually call it once to set the initial viewport size
    framebuffer_size_callback(m_Window, width, height);

    glewExperimental = GL_TRUE;
    // 4. Initialize GLEW (must be done after creating a valid GL context)
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        // Handle GLEW initialization failure
        return;
    }

    glEnable(GL_DEPTH_TEST);

    std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

#ifdef ENGINE_EDITOR
    // --- IMGUI INITIALIZATION ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = "../../../imgui.ini";

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
#endif

    SetupRenderQuad();
}

Window::~Window()
{
#ifdef ENGINE_EDITOR
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glDeleteVertexArrays(1, &m_QuadVAO);
    glDeleteBuffers(1, &m_QuadVBO);

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

#ifdef ENGINE_EDITOR
void Window::startImGUIFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Enable dockspace
    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; // Changed from PassthruCentralNode

    // Make sure we're not preventing docking
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDocking|
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpaceHost", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(3);

    // Create the dockspace
    ImGuiID dockspace_id = ImGui::GetID("DockSpaceHost");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

void Window::DrawImGUIControlsWindow(glm::vec3& lightPos)
{
    // Controls window - explicitly allow docking and moving
    ImGuiWindowFlags window_flags = 0; // No restrictive flags

    if (ImGui::Begin("Controls", nullptr, window_flags))
    {
        ImGui::Text("Light Position");
        ImGui::SliderFloat("X", &lightPos.x, -20.0f, 20.0f);
        ImGui::SliderFloat("Y", &lightPos.y, -20.0f, 20.0f);
        ImGui::SliderFloat("Z", &lightPos.z, -20.0f, 20.0f);

        // Debug info
        ImGui::Separator();
        ImGui::Text("Window is docked: %s", ImGui::IsWindowDocked() ? "Yes" : "No");
    }
    ImGui::End();
}

void Window::DrawSceneView(Framebuffer& framebuffer, Camera& camera, GLFWwindow* nativeWindow, AppContext& context)
{
    // A static variable to track if we are currently controlling the camera
    static bool isControllingCamera = false;

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; // Changed from PassthruCentralNode
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar|
        ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Scene", NULL, window_flags);

    bool isHovered = ImGui::IsWindowHovered();

    // --- NEW INPUT LOGIC ---

    // 1. START controlling the camera if we right-click while hovering
    if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        isControllingCamera = true;
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.isFocused = true; 
    }

    // 2. STOP controlling the camera if we release the right mouse button
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        isControllingCamera = false;
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        camera.isFocused = false;
    }

    // 3. Process mouse movement ONLY when we are in control mode
    if (isControllingCamera)
    {
        ImGuiIO& io = ImGui::GetIO();
        camera.ProcessMouseDelta(io.MouseDelta.x, io.MouseDelta.y);
    }

    // --- END NEW INPUT LOGIC ---

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint64_t textureID = framebuffer.getTextureColorBuffer();
    ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGuiID dockspace_id = ImGui::GetID("DockSpaceHost");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
    ImGui::PopStyleVar();
}

void Window::ImGUIRender()
{
    // End the DockSpaceHost window. This concludes the scope for dockable windows.
    ImGui::End();

    // Final ImGui rendering
    ImGui::Render();    
    int display_w, display_h;
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    swapBuffers();
}
#endif

void Window::swapBuffers()
{
    // Swaps the front and back buffers
    glfwSwapBuffers(m_Window);
}

void Window::SetupRenderQuad()
{
    // 1. Create VAO and VBO
    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);

    // 2. Bind them and upload vertex data
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_QuadVertices), &m_QuadVertices, GL_STATIC_DRAW);

    // 3. Set up vertex attribute pointers
    // The vertex shader expects position at location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // The vertex shader expects texture coordinates at location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Window::pollEvents()
{
    // Poll for and process events
    glfwPollEvents();
}

GLFWwindow* Window::getNativeWindow() const
{
    return m_Window;
}

void Window::RenderWindow(Shader& renderShader, Framebuffer& framebuffer)
{
#ifdef ENGINE_EDITOR
    ImGUIRender();
#else
    int display_w, display_h;
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderShader.use();
    glBindVertexArray(m_QuadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureColorBuffer());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    swapBuffers();
    glEnable(GL_DEPTH_TEST);
#endif
}



