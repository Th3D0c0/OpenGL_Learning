#include "Texture.h"
#include "stb_image.h"
#include <stdio.h>
#include <iostream>

// Updated Constructor
Texture::Texture(const std::string& fileLoc, const std::string& typeName)
	: m_TextureID(0), m_Width(0), m_Height(0), m_BitDepth(0),
	m_FileLocation(fileLoc), m_Type(typeName)
{
}

Texture::~Texture()
{
	ClearTexture();
}

bool Texture::LoadTexture()
{
	// OpenGL expects textures to start at the bottom-left, but most image formats
	// store them from the top-left. This flips the image vertically during loading.
	stbi_set_flip_vertically_on_load(true);

	unsigned char* textData = stbi_load(m_FileLocation.c_str(), &m_Width, &m_Height, &m_BitDepth, 0);
	if (!textData)
	{
		printf("Failed to find texture: %s\n", m_FileLocation.c_str());
		return false;
	}

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	// FIX: Determine the correct image format instead of assuming RGBA
	GLenum format = GL_RGB;
	if (m_BitDepth == 4)
	{
		format = GL_RGBA;
	}
	else if (m_BitDepth == 3)
	{
		format = GL_RGB;
	}
	else if (m_BitDepth == 1)
	{
		format = GL_RED; // For grayscale images
	}


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Use mipmaps for better quality rendering of textures at a distance
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Tell OpenGL the correct format of the loaded image data
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, textData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(textData);
	return true;
}

void Texture::bind(GLuint textureUnit) const
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::ClearTexture()
{
	if (m_TextureID != 0)
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}
	m_Width = 0;
	m_Height = 0;
	m_BitDepth = 0;
	m_FileLocation = "";
}
