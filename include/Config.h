#include "Window.h"
#include "ShaderClass/Shader.h"
#include "Scene/Mesh/Mesh.h"
#include "Scene/Object.h"
#include "Scene/Camera.h"
#include "Scene/Sphere.h"
#include "Framebuffer.h"
#include "ParticleSystem/ParticleSystem.h"
#include "PlanetGenerator/PlanetGen.h"
#include "Scene/Skybox/Skybox.h"
#include "Scene/Transform.h"
#include "Scene/Scene.h"
#include "Scene/Mesh/MeshData.h"
#include "Scene/Light.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h> 

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>