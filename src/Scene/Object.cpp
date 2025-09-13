#include "Scene/Object.h"
#include <iostream>


Object::Object(std::string filePath, std::shared_ptr<MaterialManager> materialManager)
	:m_MaterialManager(materialManager),
	m_MeshNameCounter(0)
{
	loadModel(filePath);
}

Object::~Object()
{
}

void Object::Draw(DrawProperties& properties) const
{
	for (const Mesh& mesh: m_Meshes)
	{
		mesh.Draw(properties);
	}
}

void Object::DrawObjectDepthPrepass(DrawProperties& properties)
{
	for (Mesh& mesh : m_Meshes)
	{
		glm::mat4 meshLocalMatrix = mesh.GetModelMatrix();

		// Calculate the final world matrix for this specific mesh
		glm::mat4 finalMatrix = this->m_Transform.GetModelMatrix() * meshLocalMatrix;

		// Set the final model matrix uniform for this mesh
		properties.shader->setUniformValue("model", finalMatrix);
		mesh.DrawMeshDepthPrepass(properties);
	}
}

void Object::SetLocation(const glm::vec3& location)
{
	m_Transform.SetLocation(location);
}

void Object::SetRotation(const glm::vec3& rotation)
{
	m_Transform.SetRotation(rotation);
}

void Object::SetScale(const glm::vec3& scale)
{
	m_Transform.SetScale(scale);
}

const std::vector<Mesh>& Object::GetMeshes() const
{
	return m_Meshes;
}

void Object::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	m_Directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Object::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(processMesh(mesh, scene));
		if (m_MeshNameCounter++ == 0)
		{
			std::string meshName = std::string(mesh->mName.C_Str());

			if (!meshName.empty())
			{
				std::cout << "Processing mesh named: " << meshName << std::endl;
			}
		}
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Object::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		// --- Process Vertex Positions ---
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// --- Process Normals ---
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// --- Process Texture Coordinates ---
		if (mesh->mTextureCoords[0]) // Check if the mesh has texture coordinates
		{
			// --- Process Normal Coords ---
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TextCoord = vec;

			// --- Process Tangents ---
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			// --- Process Bitangents ---
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
		{
			vertex.TextCoord = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process material
	std::shared_ptr<Material> meshMaterial;
	if (scene->mNumMaterials > 0 && mesh->mMaterialIndex < scene->mNumMaterials)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// Use the manager to get a shared material instance
		meshMaterial = m_MaterialManager->GetOrCreateMaterial(material);
	}
	else
	{
		meshMaterial = m_MaterialManager->CreateDefaultMaterial();
	}
	return Mesh(vertices, indices, meshMaterial);
}

