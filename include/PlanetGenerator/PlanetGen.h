#pragma once

#include <vector>
#include <memory>
#include "glm/vec3.hpp"
#include "Scene/Mesh/MeshData.h"
#include "ShaderClass/Shader.h"
#include "Scene/Transform.h"
#include "FastNoiseLite.h"
#include "Scene/Mesh/MeshData.h"
#include "Material/Material.h"

class Planet
{
public:
	Planet();
	~Planet();

	// initial Planet loading
	void LoadMesh(float radius, unsigned int resolution);
	void Draw(DrawProperties& properties) const;
	void DrawPlanetDepthPrepass(DrawProperties& properties);

	void SetLocation(const glm::vec3& location);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	void SetNoiseFrequency(float frequency1, float frequency2, float frequency3);

	uint32_t GetFeatureFlag();

private:
	Transform m_Transform;
	Material m_Material;

	std::vector<float> CreateSphereDensityMap(float radius, unsigned int resolution);
	uint8_t GetTableIndex(glm::ivec3 CubePos, float isoLevel);
	uint8_t CalculateCubeIndex(const float cornerDensities[8], float isoLevel);
	glm::vec3 InterpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2, float isolevel);
	glm::vec3 CalculateNormal(const glm::vec3& pos);
	float GetDensity(unsigned int x, unsigned int y, unsigned int z);

	float GetDensityTrilinear(float x, float y, float z);

	void UpdateMeshBuffers();
	void SetupMesh();

	glm::mat4 TBN;
	std::vector<float> m_DensityValues;
	int m_CurrentResolution;

	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;

	FastNoiseLite m_Noise00;
	FastNoiseLite m_Noise01;

	glm::vec3 m_SpecularPower;

	float m_Radius;
};