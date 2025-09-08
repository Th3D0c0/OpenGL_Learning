#version 460 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

// Same Light struct as the compute shader
struct Light {
    vec4 positionVS;
    vec4 color;
    vec4 directionVS;
    float spotlightAngle;
    float range;
    float intensity;
    int type;
    int enabled;
};

// Uniforms
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1; // Example material property

// SSBOs with the light culling results
layout(std140, binding = 0) readonly buffer AllLights {
    Light lights[];
};

// We will expand this to hold tile information
layout(std430, binding = 1) readonly buffer LightGrid {
    uint visibleLightCount;
    // ... tile data
};

layout(std430, binding = 2) readonly buffer VisibleLightIndices {
    uint indices[];
};

void main()
{
    // For now, we'll loop through ALL lights to test.
    // In the next step, we'll add the logic to use the light grid.
    vec3 norm = normalize(fs_in.Normal);
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 lighting = vec3(0.0); // Start with no light

    // --- (Placeholder) Loop through all lights ---
    // We will replace this with a loop over the culled list.
    for (uint i = 0; i < lights.length(); ++i) {
        // Simple Blinn-Phong lighting calculation
        vec3 lightDir = normalize(lights[i].positionVS.xyz - fs_in.FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].color.rgb * lights[i].intensity * diff * color;
        lighting += diffuse;
    }
    FragColor = vec4(lighting, 1.0);
}