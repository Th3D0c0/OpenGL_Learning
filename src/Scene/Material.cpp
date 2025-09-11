#include "Scene/Mesh/Material.h"

unsigned int Material::s_MaterialCount = 0;
Material::Material()
	:m_MaterialFlags(0)
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
	return m_MaterialFlags;
}



