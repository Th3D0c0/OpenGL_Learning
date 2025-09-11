#pragma once

#include <map>
#include "ShaderClass/Shader.h"
#include "Scene/Object.h"
#include "PlanetGenerator/PlanetGen.h"

class ShaderCache
{
public:
	ShaderCache();
	~ShaderCache();

	Shader* GetShader(uint32_t shaderFlags);
private:
	//       ShaderFlag, ShaderID
	std::map<uint32_t, std::unique_ptr<Shader>> m_ProgramCache;
};