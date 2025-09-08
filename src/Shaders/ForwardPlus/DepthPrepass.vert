#version 460 core

layout(location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{
	gl_Position = perspective * view * model * vec4(aPosition, 1.0f);
}
