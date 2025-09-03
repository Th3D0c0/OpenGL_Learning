#pragma once

#include <vector>
#include <memory>
#include "glm/vec3.hpp"
#include "MeshData.h"
#include "Shader.h"
#include "Transform.h"

class Planet
{
public:
	Planet();
	~Planet();

	// initial Planet loading
	void LoadMesh(float radius, unsigned int resolution);
	void DrawPlanet(glm::mat4 viewMat4, glm::mat4 projMat4, const glm::vec3& lightPos, const glm::vec3& viewPos);

	void SetLocation(const glm::vec3& location);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

private:
	Transform transform;

	std::vector<float> CreateSphereDensityMap(float radius, unsigned int resolution);
	uint8_t GetTableIndex(glm::ivec3 CubePos, float isoLevel);
	uint8_t CalculateCubeIndex(const float cornerDensities[8], float isoLevel);
	glm::vec3 InterpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2, float isolevel);
	glm::vec3 CalculateNormal(const glm::vec3& pos);
	float GetDensity(unsigned int x, unsigned int y, unsigned int z);

	void UpdateMeshBuffers();
	void SetupMesh();


	std::vector<float> m_DensityValues;
	int m_CurrentResolution;

	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;

	std::unique_ptr<Shader> m_Shader;
};;