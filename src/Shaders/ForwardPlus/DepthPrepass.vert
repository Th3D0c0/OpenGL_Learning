#version 460 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;

// Uniforms for transformations
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Standard model-view-projection transformation.
    // The only goal of this pass is to write to the depth buffer.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
