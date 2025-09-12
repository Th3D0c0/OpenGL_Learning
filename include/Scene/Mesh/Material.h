#pragma once

#include <vector>
#include "Texture.h"
#include "../../../out/build/x64-Debug/_deps/assimp-src/code/AssetLib/glTF2/glTF2Exporter.h"
#include "glm/glm.hpp"

class Material
{
public:
	Material();
	~Material();
	unsigned int GetID();
	uint32_t GetFeatureFlag();

	void DisableFeatureFlags();
	void SetDiffuseMapEnabled(bool value);
	void SetNormalMapEnabled(bool value);
	void SetSpecularMapEnabled(bool value);
	void SetAlphaTestEnabled(bool value);

	void SetNormalMapStrength(float strength);
	void SetNormalMapScaling(float scaling);
	void SetSpecularPower(float strength);
	void SetDiffuseColor(glm::vec3 color);

	void CreateDiffuseTextureAndLoad(std::string imagePath);
	void CreateNormalMapAndLoad(std::string imagePath);
	void CreateSpecularMapAndLoad(std::string imagePath);

	std::vector<Texture*> GetAllTextures();
	float GetNormalMapScaling();

private:
	static unsigned int s_MaterialCount;

	unsigned int m_ID;

	std::vector<Texture> m_DiffuseMap;
	std::vector<Texture> m_SpecularMap;
	std::vector<Texture> m_NormalMap;
	glm::vec3 m_DiffuseColor;
	float m_SpecularPower;
	float m_NormalPower;
	float m_NormalScaling;

	uint32_t m_MaterialFlag;
};