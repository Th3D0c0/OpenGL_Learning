#include "ShaderClass/ShaderCache.h"

ShaderCache::ShaderCache()
{
}

ShaderCache::~ShaderCache()
{
}

Shader* ShaderCache::GetShader(uint32_t shaderFlags)
{
	auto it = m_ProgramCache.find(shaderFlags);

	if (it == m_ProgramCache.end())
	{
		auto newShader = std::make_unique<Shader>(shaderFlags);
		it = m_ProgramCache.emplace(shaderFlags, std::move(newShader)).first;
	}
	return it->second.get();
}

