#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"

class Skybox
{
public:
    // Constructor
    Skybox();

    // Load the skybox textures from file paths
    void load(const std::vector<std::string>& faces);

    // Draw the skybox
    void draw(const glm::mat4& view, const glm::mat4& projection);
    unsigned int getCubemapTexture() const { return cubemapTexture; }

private:
    unsigned int vao, vbo;
    unsigned int cubemapTexture;
    Shader* skyboxShader;

    // Setup the cube mesh (VAO, VBO)
    void setupMesh();
};