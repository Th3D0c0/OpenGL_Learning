#version 460 core
// This fragment shader's job is to sample a color from a texture
// using the incoming texture coordinates and output it.

// Output color for the pixel
out vec4 FragColor;

// Input from the vertex shader
in vec2 TexCoords;

// The texture we want to draw (our framebuffer's color attachment)
uniform sampler2D screenTexture;

void main()
{
    // Sample the color from the texture at the given coordinate.
    FragColor = texture(screenTexture, TexCoords);
}
