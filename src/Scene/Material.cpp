#include "Scene/Mesh/Material.h"

#include "../../out/build/x64-Debug/_deps/assimp-src/code/AssetLib/Blender/BlenderScene.h"
#include "ShaderClass/FeatureFlags.h"

unsigned int Material::s_MaterialCount = 0;
Material::Material()
	:m_MaterialFlag(0),
	m_NormalScaling(1.0f)
{
	s_MaterialCount++;
	m_ID = s_MaterialCount;
}

Material::~Material()
{
	s_MaterialCount--;
}

unsigned int Material::GetID()
{
	return m_ID;
}

uint32_t Material::GetFeatureFlag()
{
	return m_MaterialFlag;
}

void Material::DisableFeatureFlags()
{
	m_MaterialFlag = 0;
}

void Material::SetDiffuseMapEnabled(bool value)
{
	if (value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_Diffuse_MAP) == 0)
		{
			m_MaterialFlag |= SHADER_FEATURE_Diffuse_MAP;
		}
	}
	if (!value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_Diffuse_MAP) != 0)
		{
			m_MaterialFlag &= ~SHADER_FEATURE_Diffuse_MAP;
		}
	}
}

void Material::SetNormalMapEnabled(bool value)
{
	if (value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_NORMAL_MAP) == 0)
		{
			m_MaterialFlag |= SHADER_FEATURE_NORMAL_MAP;
		}
	}
	if (!value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_NORMAL_MAP) != 0)
		{
			m_MaterialFlag &= ~SHADER_FEATURE_NORMAL_MAP;
		}
	}
}

void Material::SetSpecularMapEnabled(bool value)
{
	if (value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_SPECULAR_MAP) == 0)
		{
			m_MaterialFlag |= SHADER_FEATURE_SPECULAR_MAP;
		}
	}
	if (!value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_SPECULAR_MAP) != 0)
		{
			m_MaterialFlag &= ~SHADER_FEATURE_SPECULAR_MAP;
		}
	}
}

void Material::SetAlphaTestEnabled(bool value)
{
	if (value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_ALPHA_TEST) == 0)
		{
			m_MaterialFlag |= SHADER_FEATURE_ALPHA_TEST;
		}
	}
	if (!value)
	{
		if ((m_MaterialFlag & SHADER_FEATURE_ALPHA_TEST) != 0)
		{
			m_MaterialFlag &= ~SHADER_FEATURE_ALPHA_TEST;
		}
	}
}

void Material::SetNormalMapStrength(float strength)
{
	float clampedStrength = glm::clamp(strength, 0.0f, strength);
	m_NormalPower = clampedStrength;
}

void Material::SetNormalMapScaling(float scaling)
{
	float clampedScaling = glm::clamp(scaling, 0.0f, scaling);
	m_NormalScaling = clampedScaling;
}

void Material::SetSpecularPower(float strength)
{
	float clampedStrength = glm::clamp(strength, 0.0f, strength);
	m_SpecularPower = clampedStrength;
}

void Material::SetDiffuseColor(glm::vec3 color)
{
	glm::vec3 clampedcolor = glm::clamp(color, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_DiffuseColor = clampedcolor;
}

void Material::CreateDiffuseTextureAndLoad(std::string imagePath)
{
	m_DiffuseMap.emplace_back(imagePath, "texture_diffuse");
	m_DiffuseMap.back().LoadTexture();
}

void Material::CreateNormalMapAndLoad(std::string imagePath)
{
	m_NormalMap.emplace_back(imagePath, "texture_normal");
	m_NormalMap.back().loadNormalTexture();
}

void Material::CreateSpecularMapAndLoad(std::string imagePath)
{
	m_SpecularMap.emplace_back(imagePath, "texture_specular");
	m_SpecularMap.back().LoadTexture();
}

std::vector<Texture*> Material::GetAllTextures()
{
	std::vector<Texture*> outTextures;
	for (Texture& tex : m_DiffuseMap)
	{
		outTextures.push_back(&tex);
	}
	for (Texture& tex : m_NormalMap)
	{
		outTextures.push_back(&tex);
	}
	for (Texture& tex : m_SpecularMap)
	{
		outTextures.push_back(&tex);
	}
	return outTextures;
}

float Material::GetNormalMapScaling()
{
	return m_NormalScaling;
}





