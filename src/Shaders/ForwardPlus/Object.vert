#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 FragPosView;  // Changed from FragPosWorld
    vec3 NormalView;   // Changed from NormalWorld
    vec3 FragPosWorld;
    vec3 NormalWorld;
    vec2 TexCoords;
} vs_out;

out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Combine model and view matrices
    mat4 modelView = view * model;

    // Transform position to view space
    vs_out.FragPosView = vec3(modelView * vec4(aPos, 1.0f));
    vs_out.FragPosWorld = vec3(model * vec4(aPos, 1.0f));

    // Transform normal to view space using the inverse transpose of the model-view matrix
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vs_out.NormalView = normalize(normalMatrix * aNormal);
    mat3 normalMatrixWorld = mat3(transpose(inverse(model)));
    vs_out.NormalWorld = normalize(normalMatrixWorld * aNormal);

    vec3 T = normalize(mat3(modelView) * aTangent);
    vec3 N = normalize(mat3(modelView) * aNormal); // This is vs_out.NormalView
    T = normalize(T - dot(T, N) * N); // Re-orthogonalize Tangent
    vec3 B = cross(N, T); // Calculate Bitangent
    vs_out.TBN = mat3(T, B, N);

    // Pass through texture coordinates
    vs_out.TexCoords = aTexCoords;
    
    // Final position for rasterization
    gl_Position = projection * modelView * vec4(aPos, 1.0f);
}