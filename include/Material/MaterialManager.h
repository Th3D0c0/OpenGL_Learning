#pragma once

#include <map>
#include <memory>
#include "Material/Material.h"


class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	std::shared_ptr<Material> GetOrCreateMaterial(aiMaterial* assimpMaterial);
	std::shared_ptr<Material> CreateDefaultMaterial();

private:
	std::map<std::string, std::shared_ptr<Material>> m_Materials;
};