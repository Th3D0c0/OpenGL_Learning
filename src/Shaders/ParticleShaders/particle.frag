#version 460 core

out vec4 FragColor;

// Inputs from the vertex shader
in vec3 Normal;
in vec2 TextCoords;
in vec4 particleColor;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

// New uniforms for lighting
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // --- Ambient ---
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // --- Diffuse ---
    // Calculate the direction from the fragment to the light source
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    // Calculate the diffuse impact (how much the fragment is facing the light)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // --- Specular ---
    float specularStrength = 0.5;
    // Calculate the direction from the fragment to the camera (viewer)
    vec3 viewDir = normalize(viewPos - FragPos);
    // Calculate the reflection of the light off the surface
    vec3 reflectDir = reflect(-lightDir, norm);
    // Calculate the specular highlight
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 textureColor = texture(texture_diffuse1, TextCoords);
    // --- Final Color ---
    vec3 result = (ambient + diffuse + specular) * particleColor.rgb;
    FragColor = vec4(result, 1.0f);
}