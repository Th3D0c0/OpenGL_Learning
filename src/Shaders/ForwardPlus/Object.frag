// Object.frag (Modified for View-Space Lighting)
#version 460 core

// <DEFINES_PLACEHOLDER>

out vec4 FragColor;

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

// --- Inputs / Uniforms ---
in VS_OUT {
    vec3 FragPosView;   // Changed from FragPosWorld
    vec3 NormalView;    // Changed from NormalWorld
    vec3 FragPosWorld;
    vec3 NormalWorld;
    vec2 TexCoords;
} fs_in;

in mat3 TBN;

#ifdef USE_DiffuseMap
uniform sampler2D texture_diffuse1;
#endif

#ifdef USE_NormalMap
uniform sampler2D texture_normal1;
uniform float normalTextureScale;
#endif

//vec3 baseColor = texture(texture_diffuse1, fs_in.TexCoords).rgb;
vec3 baseColor = vec3(0.3, 0.4, 0.5);

uniform vec3 ambientColor;
uniform float specularPower;
uniform uint TILE_SIZE;
uniform mat4 view;
// --- Structs (must be identical everywhere) ---
struct Light {
    vec4 positionVS;    // RENAMED: Data is expected in view space
    vec4 directionVS;   // RENAMED: Data is expected in view space
    vec4 color;
    float spotAngle;
    float range;
    float intensity;
    int type;
};

struct Material {
    float specularPower;
};

struct LightingResult {
    vec3 diffuse;
    vec3 specular;
};

// --- Buffers ---
layout(std430, binding = 0) readonly buffer LightsBuffer { Light lights[]; };
layout(std430, binding = 2) readonly buffer LightIndexBuffer { uint lightIndices[]; };
layout(rg32ui, binding = 4) uniform readonly uimage2D lightGrid;

// --- Helper Functions ---

// Calculates the diffuse contribution
vec3 DoDiffuse(Light light, vec3 L, vec3 N) {
    float NdotL = max(dot(N, L), 0.0);
    return light.color.rgb * light.intensity * NdotL;
}

// Calculates the specular contribution
vec3 DoSpecular(Light light, Material mat, vec3 V, vec3 L, vec3 N) {
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    
    if (mat.specularPower <= 1.0) return vec3(0.0);
    return light.color.rgb * light.intensity * pow(RdotV, mat.specularPower);
}

// Calculates attenuation for point lights
float CalculateAttenuation(float distance, float range) {
    float attenuation = 1.0 / (1.0 + distance * distance / (range * range));
    float cutoff = 1.0 - smoothstep(range * 0.8, range, distance);
    return attenuation * cutoff;
}

// Calculates contribution for a Point Light (all in view space)
LightingResult DoPointLight(Light light, Material mat, vec3 V, vec3 P, vec3 N) {
    LightingResult result;
    vec3 lightPosView = light.positionVS.xyz; // Use view space position
    vec3 L = lightPosView - P;
    float distance = length(L);
    L = normalize(L);
    
    if (distance > light.range) {
        result.diffuse = vec3(0.0);
        result.specular = vec3(0.0);
        return result;
    }
    
    float attenuation = CalculateAttenuation(distance, light.range);
    if (attenuation <= 0.001) {
        result.diffuse = vec3(0.0);
        result.specular = vec3(0.0);
        return result;
    }
    
    result.diffuse = DoDiffuse(light, L, N) * attenuation;
    result.specular = DoSpecular(light, mat, V, L, N) * attenuation;
    
    return result;
}

// Calculates contribution for a Spot Light (all in view space)
LightingResult DoSpotLight(Light light, Material mat, vec3 V, vec3 P, vec3 N) {
    LightingResult result;
    vec3 lightPosView = light.positionVS.xyz; // Use view space position
    vec3 lightDirView = normalize(light.directionVS.xyz); // Use view space direction
    
    vec3 L = lightPosView - P;
    float distance = length(L);
    L = normalize(L);
    
    if (distance > light.range) {
        result.diffuse = vec3(0.0);
        result.specular = vec3(0.0);
        return result;
    }
    
    float spotEffect = dot(-L, lightDirView);
    float spotCutoff = cos(light.spotAngle);
    
    if (spotEffect < spotCutoff) {
        result.diffuse = vec3(0.0);
        result.specular = vec3(0.0);
        return result;
    }
    
    float spotAttenuation = smoothstep(spotCutoff, spotCutoff + 0.1, spotEffect);
    float attenuation = CalculateAttenuation(distance, light.range) * spotAttenuation;
    if (attenuation <= 0.001) {
        result.diffuse = vec3(0.0);
        result.specular = vec3(0.0);
        return result;
    }
    
    result.diffuse = DoDiffuse(light, L, N) * attenuation;
    result.specular = DoSpecular(light, mat, V, L, N) * attenuation;
    
    return result;
}

// Calculates contribution for a Directional Light (all in view space)
LightingResult DoDirectionalLight(Light light, Material mat, vec3 V, vec3 P, vec3 N) {
    LightingResult result;
    vec3 lightDirView = normalize(light.directionVS.xyz); // Use view space direction
    vec3 L = -lightDirView;
    
    result.diffuse = DoDiffuse(light, L, N);
    result.specular = DoSpecular(light, mat, V, L, N);
    
    return result;
}

// Function to perform triplanar texture sampling
vec4 triplanar(sampler2D tex, vec3 worldPos, vec3 worldNormal, float scale) {
    // 1. Calculate blend weights
    // Use the absolute value of the normal to handle all directions
    vec3 blendWeights = abs(worldNormal);
    
    // Tweak the blend weights to reduce blurriness.
    // Raising to a power sharpens the transition between textures.
    blendWeights = pow(blendWeights, vec3(2.0));
    
    // Normalize the blend weights so they sum to 1.
    // This ensures consistent brightness. Add a small epsilon to avoid division by zero.
    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z + 0.00001);

    // 2. Sample the texture from three different planes (X, Y, Z)
    // The texture coordinates are generated from the world position.
    vec2 texCoordsX = worldPos.yz * scale;
    vec2 texCoordsY = worldPos.xz * scale;
    vec2 texCoordsZ = worldPos.xy * scale;
    
    vec4 xSample = texture(tex, texCoordsX);
    vec4 ySample = texture(tex, texCoordsY);
    vec4 zSample = texture(tex, texCoordsZ);

    // 3. Blend the three samples based on the weights
    vec4 finalColor = xSample * blendWeights.x + 
                      ySample * blendWeights.y + 
                      zSample * blendWeights.z;
    
    return finalColor;
}

// Main Calculation Loop
void main()
{
    // Everything is now in view space
    vec3 N_viewSpace;

    #ifdef USE_NormalMap
    // 1. Get the raw tangent-space normal from the triplanar map
    vec4 triplanarNormalColor = triplanar(texture_normal1, fs_in.FragPosWorld, fs_in.NormalWorld, normalTextureScale);

    // 2. Unpack the normal from [0,1] to [-1,1]
    vec3 normalTangentSpace = normalize(triplanarNormalColor.rgb * 2.0 - 1.0);

    // 3. Transform from TANGENT space to VIEW space using the TBN matrix
    N_viewSpace = normalize(TBN * normalTangentSpace);
    #else
    // If not using a normal map, the interpolated normal is already in view space
    N_viewSpace = normalize(fs_in.NormalView);
    #endif

    vec3 P = fs_in.FragPosView;
    
    // In view space, the camera is at the origin (0,0,0)
    // The vector from the fragment to the camera is simply the negation of the fragment's position vector
    vec3 V = normalize(-P); 
    
    Material mat;
    mat.specularPower = specularPower;
 
    vec3 finalColor = ambientColor * baseColor;

    ivec2 tileCoords = ivec2(gl_FragCoord.xy) / int(TILE_SIZE);
    ivec2 gridSize = imageSize(lightGrid);
    if (tileCoords.x >= gridSize.x || tileCoords.y >= gridSize.y || 
        tileCoords.x < 0 || tileCoords.y < 0) {
        FragColor = vec4(finalColor, 1.0);
        return;
    }
    
    uvec2 lightData = imageLoad(lightGrid, tileCoords).xy;
    uint offset = lightData.x;
    uint count = lightData.y;
    
    for (uint i = 0; i < count; ++i)
    {
        uint lightIndex = lightIndices[offset + i];
        
        // Bounds check for light index
        if (lightIndex >= lights.length()) continue;
        
        Light light = lights[lightIndex];
        LightingResult result;

        // Calculate lighting based on light type
        if (light.type == 0) {          // Point Light
            result = DoPointLight(light, mat, V, P, N_viewSpace);
        } else if (light.type == 1) {   // Spot Light
            result = DoSpotLight(light, mat, V, P, N_viewSpace);
        } else if (light.type == 2) {   // Directional Light
            result = DoDirectionalLight(light, mat, V, P, N_viewSpace);
        } else {
            // Unknown light type
            result.diffuse = vec3(0.0);
            result.specular = vec3(0.0);
        }
        finalColor += (result.diffuse * baseColor) + result.specular;
    }
    FragColor = vec4(finalColor, 1.0);
}