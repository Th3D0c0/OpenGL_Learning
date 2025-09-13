#include "Material/MaterialManager.h"
#include <iostream>

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

std::shared_ptr<Material> MaterialManager::GetOrCreateMaterial(aiMaterial* assimpMaterial)
{
    // Use Assimp's material name as a unique key
    std::string name = assimpMaterial->GetName().C_Str();

    // 1. Check if the material already exists in our map
    if (m_Materials.find(name) != m_Materials.end())
    {
        // 2. If it exists, just return the shared pointer to it
        return m_Materials[name];
    }

    // 3. If it doesn't exist, we need to create it
    std::shared_ptr<Material> newMaterial = std::make_shared<Material>();
    aiString texturePath; // Assimp's string type
    // Find Diffuse Map or use Diffuse Color
    aiReturn result = assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
    if (result == AI_SUCCESS)
    {
        // Successfully found a texture path.
        // Convert aiString to std::string for easier use.
        std::string finalPath = std::string(texturePath.C_Str());
        newMaterial->CreateDiffuseTextureAndLoad(finalPath);
    }
    else
    {
        aiColor3D diffuseColor; // Try to get the diffuse texture path
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        newMaterial->SetDiffuseColor(glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));
    }

    // Find Normal map 
	result = assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
    if (result == AI_SUCCESS)
    {
        // Successfully found a texture path.
        // Convert aiString to std::string for easier use.
        std::string finalPath = std::string(texturePath.C_Str());
        newMaterial->CreateNormalMapAndLoad(finalPath);
    }

    // Find Specular map 
    result = assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
    if (result == AI_SUCCESS)
    {
        // Successfully found a texture path.
        // Convert aiString to std::string for easier use.
        std::string finalPath = std::string(texturePath.C_Str());
        newMaterial->CreateSpecularMapAndLoad(finalPath);
    }

    // 4. Store the newly created material in the map and return it
    m_Materials[name] = newMaterial;
    return newMaterial;
}

std::shared_ptr<Material> MaterialManager::CreateDefaultMaterial()
{
     // Use a unique name to cache the default material
    std::string name = "DEFAULT_MATERIAL";

    // If it already exists, just return it
    if (m_Materials.find(name) != m_Materials.end())
    {
        return m_Materials[name];
    }

    // Otherwise, create a new simple, grey material
    std::shared_ptr<Material> defaultMaterial = std::make_shared<Material>();
    defaultMaterial->SetDiffuseColor(glm::vec3(0.6f, 0.6f, 0.6f));
    // Set its feature flags appropriately here!

    // Store it in the cache and return it
    m_Materials[name] = defaultMaterial;
    return defaultMaterial;
}
