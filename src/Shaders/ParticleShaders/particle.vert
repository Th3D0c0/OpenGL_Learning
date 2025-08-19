#version 460

// Mesh attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoord;

// Per instance attributes
layout (location = 3) in vec3 aParticlePosition;
layout (location = 4) in vec4 aParticleColor;
layout (location = 5) in float aParticleSize;

out vec4 particleColor;
out vec3 Normal;
out vec2 TextCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    vec3 finalPos = aPos * aParticleSize + aParticlePosition;

    FragPos = vec3(model * vec4(finalPos, 1.0f));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TextCoords = vec2(aTextCoord);
    particleColor = aParticleColor; 

    gl_Position = projection * view * vec4(FragPos, 1.0f);
}