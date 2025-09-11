#pragma once

#include <vector>
#include "Texture.h"
#include "glm/glm.hpp"

class Material
{
public:
	Material();
	~Material();
	unsigned int GetID();
	uint32_t GetFeatureFlag();

private:
	static unsigned int s_MaterialCount;

	unsigned int m_ID;

	std::vector<Texture> m_DiffuseMap;
	std::vector<Texture> m_SpecularMap;
	glm::vec3 m_DiffuseColor;
	float specularPower;

	uint32_t m_MaterialFlags;
};