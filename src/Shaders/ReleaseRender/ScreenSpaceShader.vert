#version 460 core
// This vertex shader is extremely simple. Its only job is to pass through
// vertex positions and texture coordinates for a 2D quad.

// Input vertex data: position and texture coordinates (UVs)
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

// Output to the fragment shader
out vec2 TexCoords;

void main()
{
    // The vertex positions are already in Normalized Device Coordinates (-1 to 1),
    // so we can pass them directly to gl_Position.
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 

    // Pass the texture coordinates to the fragment shader.
    TexCoords = aTexCoords;
}