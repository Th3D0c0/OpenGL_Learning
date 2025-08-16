#pragma once
#include <GL/glew.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& fileLoc, const std::string& typeName);
    ~Texture();

    // Loads the texture from the file path provided in the constructor
    bool LoadTexture();

    // Binds the texture to a specific texture unit
    void bind(unsigned int textureUnit) const;
    void unbind() const;

    // Getters
    unsigned int getID() const { return m_TextureID; }
    const std::string& getType() const { return m_Type; }

private:
    void ClearTexture();

    unsigned int m_TextureID;
    int m_Width, m_Height, m_BitDepth;
    std::string m_FileLocation;
    std::string m_Type; // e.g., "texture_diffuse", "texture_specular"
};