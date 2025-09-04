#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    // Sample the cubemap using the vertex position as the direction vector.
    FragColor = texture(skybox, TexCoords);
}
