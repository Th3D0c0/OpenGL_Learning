#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPosView;  // Changed from FragPosWorld
    vec3 NormalView;   // Changed from NormalWorld
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Combine model and view matrices
    mat4 modelView = view * model;

    // Transform position to view space
    vs_out.FragPosView = vec3(modelView * vec4(aPos, 1.0));

    // Transform normal to view space using the inverse transpose of the model-view matrix
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vs_out.NormalView = normalize(normalMatrix * aNormal);

    // Pass through texture coordinates
    vs_out.TexCoords = aTexCoords;
    
    // Final position for rasterization
    gl_Position = projection * modelView * vec4(aPos, 1.0);
}