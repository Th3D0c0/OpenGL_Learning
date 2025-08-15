#include "Window.h"
#include <iostream>



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update OpenGL's viewport to match the new window size
    glViewport(0, 0, width, height);
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
    ;
    if (!m_Window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        // Handle window creation failure
        return;
    }
    glfwMakeContextCurrent(m_Window);

    glfwSwapInterval(1);

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

    // --- IMGUI INITIALIZATION ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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
}

Window::~Window()
{
    // Clean up resources
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

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
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;
    // Removed ImGuiWindowFlags_MenuBar if you're not using it

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

void Window::DrawImGUIDockspace(Framebuffer& SceneFramebuffer, glm::vec2& SceneViewportSize)
{
    ImGui::Begin("Scene");
    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (SceneViewportSize.x != avail.x || SceneViewportSize.y != avail.y)
    {
        SceneViewportSize = {avail.x, avail.y};
        if (SceneViewportSize.x > 0 && SceneViewportSize.y > 0)
        {
            SceneFramebuffer.resize((int)SceneViewportSize.x, (int)SceneViewportSize.y);
        }
    }


    uint64_t textureID = SceneFramebuffer.getTextureID(); // assumes GL texture id
    ImGui::Image(
        (void*)(intptr_t)textureID,                                    // safe cast
        ImVec2 {SceneViewportSize.x, SceneViewportSize.y},
        ImVec2 {0, 1}, ImVec2 {1, 0}                                 // flip vertically if needed
    );

    static bool sceneHovered = false;
    sceneHovered = ImGui::IsItemHovered();

    ImGui::End();

}

void Window::DrawImGUIControlsWindow(glm::vec3& lightPos)
{
    // Controls window - explicitly allow docking and moving
    ImGuiWindowFlags window_flags = 0; // No restrictive flags

    if (ImGui::Begin("Controls", nullptr, window_flags))
    {
        ImGui::Text("Light Position");
        ImGui::SliderFloat("X", &lightPos.x, -2.0f, 2.0f);
        ImGui::SliderFloat("Y", &lightPos.y, -2.0f, 2.0f);
        ImGui::SliderFloat("Z", &lightPos.z, -2.0f, 2.0f);

        // Debug info
        ImGui::Separator();
        ImGui::Text("Window is docked: %s", ImGui::IsWindowDocked() ? "Yes" : "No");
    }
    ImGui::End();
}

void Window::swapBuffers()
{
    // Swaps the front and back buffers
    glfwSwapBuffers(m_Window);
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
}

