#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    // The key trick: Use the view and projection matrices to transform the position.
    // Then, set the z component of the final position to be equal to its w component.
    // This forces the depth value of every skybox fragment to be 1.0 (the maximum depth),
    // ensuring it's always drawn behind everything else.
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
