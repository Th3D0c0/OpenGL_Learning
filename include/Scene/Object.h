#pragma once

#include <vector>
#include "Scene/Mesh/Mesh.h"
#include "Scene/Transform.h"
#include "Material/MaterialManager.h"
#include "ShaderClass/Shader.h"
#include "glm/glm.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Object
{
public:
	Object(std::string filePath, std::shared_ptr<MaterialManager> materialManager);
	~Object();

	void Draw(DrawProperties& properties) const;
	void DrawObjectDepthPrepass(DrawProperties& properties);

	void SetLocation(const glm::vec3& location);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	const std::vector<Mesh>& GetMeshes() const;

	DrawProperties properties;

private:
	std::vector<Mesh> m_Meshes;
	Transform m_Transform;
	std::string m_Directory;
	std::shared_ptr<MaterialManager> m_MaterialManager;
	std::string m_Name;
	uint16_t m_MeshNameCounter;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};