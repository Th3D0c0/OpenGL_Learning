#include "PlanetGenerator/PlanetGen.h"
#include"PlanetGenerator/Tables.h"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <map>

Planet::Planet()
{
	SetupMesh();
	m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
}

Planet::~Planet()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void Planet::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	m_Shader = std::make_unique<Shader>("Shaders/PlanetShaders/Planet.vert", "Shaders/PlanetShaders/Planet.frag");
}

void Planet::LoadMesh(float radius, unsigned int resolution)
{
	m_DensityValues = CreateSphereDensityMap(radius, resolution);

	m_Vertices.clear();
	m_Indices.clear();

	std::map <int, unsigned int> vertexMap;

	for (int x = 0; x < m_CurrentResolution - 1; x++)
	{
		for (int y = 0; y < m_CurrentResolution - 1; y++)
		{
			for (int z = 0; z < m_CurrentResolution - 1; z++)
			{
				glm::ivec3 cornerPositions[8];
				float cornerDensities[8];
				for (int i = 0; i < 8; ++i)
				{
					glm::ivec3 cornerPos = glm::ivec3(x, y, z) + cornerOffsets[i];
					cornerPositions[i] = cornerPos;
					cornerDensities[i] = GetDensity(cornerPos.x, cornerPos.y, cornerPos.z);
				}

				uint8_t cubeIndex = CalculateCubeIndex(cornerDensities, 0.0f);

				if (cubeIndex == 0 || cubeIndex == 255) continue;

				unsigned int triIndices[3];

				for (int i = 0; triTable[cubeIndex][i] != -1; i += 3)
				{
					for (int j = 0; j < 3; j++)
					{
						int edgeIndex = triTable[cubeIndex][i + j];

						int cornerAIndex = edgeToVertex[edgeIndex][0];
						int cornerBIndex = edgeToVertex[edgeIndex][1];

						unsigned int indexA = cornerPositions[cornerAIndex].x + cornerPositions[cornerAIndex].y * m_CurrentResolution + cornerPositions[cornerAIndex].z * m_CurrentResolution * m_CurrentResolution;
						unsigned int indexB = cornerPositions[cornerBIndex].x + cornerPositions[cornerBIndex].y * m_CurrentResolution + cornerPositions[cornerBIndex].z * m_CurrentResolution * m_CurrentResolution;

						if (indexA > indexB) std::swap(indexA, indexB);
						uint64_t edgeID = ((uint64_t)indexA << 32) | indexB;

						if (vertexMap.find(edgeID) == vertexMap.end())
						{
							glm::vec3 vertPos = InterpolateVertex(
								cornerPositions[cornerAIndex],
								cornerPositions[cornerBIndex],
								cornerDensities[cornerAIndex],
								cornerDensities[cornerBIndex],
								0.0f);

							glm::vec3 centerOffset(m_CurrentResolution / 2.0f);
							vertPos -= centerOffset;

							glm::vec3 vertNorm = CalculateNormal(vertPos + centerOffset);
							Vertex v;
							v.Position = vertPos;
							v.Normal = vertNorm;
							m_Vertices.push_back(v);

							unsigned int newIndex = m_Vertices.size() - 1;
							vertexMap[edgeID] = newIndex;
							triIndices[j] = newIndex;
						}
						else
						{
							triIndices[j] = vertexMap[edgeID];
						}
					}
					m_Indices.push_back(triIndices[0]);
					m_Indices.push_back(triIndices[1]);
					m_Indices.push_back(triIndices[2]);
				}
			}
		}
	}
	UpdateMeshBuffers();
}

void Planet::DrawPlanet(glm::mat4 viewMat4, glm::mat4 projMat4, const glm::vec3& lightPos, const glm::vec3& viewPos)
{
	m_Shader->use();
	m_Shader->setUniformValue("projection", projMat4);
	m_Shader->setUniformValue("view", viewMat4);
	m_Shader->setUniformValue("model", transform.GetModelMatrix());

	m_Shader->setUniformValue("lightPos", lightPos);
	m_Shader->setUniformValue("viewPos", viewPos);
	m_Shader->setUniformValue("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));



	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Planet::SetLocation(const glm::vec3& location)
{
	transform.SetLocation(location);
}

void Planet::SetRotation(const glm::vec3& rotation)
{
	transform.SetRotation(rotation);
}

void Planet::SetScale(const glm::vec3& scale)
{
	transform.SetScale(scale);
}


void Planet::UpdateMeshBuffers()
{
	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	// Send the new data to the GPU
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_DYNAMIC_DRAW);
}

std::vector<float> Planet::CreateSphereDensityMap(float radius, unsigned int resolution)
{
	Planet::m_CurrentResolution = resolution;

	glm::vec3 center(resolution/2.0f, resolution / 2.0f, resolution / 2.0f);
	std::vector<float> outDensities(resolution * resolution * resolution);

	float noiseStrength = radius * 0.2f;
	float scale = 0.05f;
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			for (int z = 0; z < resolution; z++)
			{
				float distToCenter = glm::distance(glm::vec3(x, y, z), center);
				float density = distToCenter - radius;

				density += m_Noise.GetNoise((float)x * scale,(float)y * scale,(float) z * scale) * noiseStrength;

				int index = x + y * resolution + z * resolution * resolution;
				outDensities[index] = density;
			}
		}
	}
	return outDensities;
}

std::vector<float> Planet::GenerateNoise(unsigned int resolution)
{
	int index = 0;
	std::vector<float> noiseData(resolution * resolution * resolution);
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			for (int z = 0; z < resolution; z++)
			{
				noiseData[index++] = m_Noise.GetNoise((float)x, (float)y, (float)z);
			}
		}
	}
	return noiseData;
}

uint8_t Planet::GetTableIndex(glm::ivec3 CubePos, float isoLevel)
{
	glm::ivec3 positionOffset[8] = {
		glm::ivec3(0,0,0),
		glm::ivec3(1,0,0),
		glm::ivec3(1,1,0),
		glm::ivec3(0,1,0),
		glm::ivec3(0,0,1),
		glm::ivec3(1,0,1),
		glm::ivec3(1,1,1),
		glm::ivec3(0,1,1),
	};

	uint8_t cubeIndex = 0;


	for (int i = 0; i < 8; i++)
	{
		glm::ivec3 cornerPos = CubePos + positionOffset[i];
		int index = cornerPos.x + cornerPos.y * Planet::m_CurrentResolution + cornerPos.z * Planet::m_CurrentResolution * Planet::m_CurrentResolution;
		float density = m_DensityValues[index];
		if (density < isoLevel)
		{
			cubeIndex |= (1 << i);
		}
	}
	return cubeIndex;
}

uint8_t Planet::CalculateCubeIndex(const float cornerDensities[8], float isoLevel)
{
	uint8_t cubeIndex = 0;
	for (int i = 0; i < 8; i++)
	{
		if (cornerDensities[i] < isoLevel)
		{
			cubeIndex |= (1 << i);
		}
	}
	return cubeIndex;
}

float Planet::GetDensity(unsigned int x, unsigned int y, unsigned int z)
{
	if (x >= m_CurrentResolution || y >= m_CurrentResolution || z >= m_CurrentResolution)
	{
		return 0.0f; 
	}
	unsigned int index = x + y * m_CurrentResolution + z * m_CurrentResolution * m_CurrentResolution;
	return m_DensityValues[index];
}

glm::vec3 Planet::InterpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2, float isolevel)
{
	if (std::abs(isolevel - d1) < 0.00001) return p1;
	if (std::abs(isolevel - d2) < 0.00001) return p2;
	if (std::abs(d1 - d2) < 0.00001) return p1;

	float t = (isolevel - d1) / (d2 - d1);
	return p1 + t * (p2 - p1);
}

glm::vec3 Planet::CalculateNormal(const glm::vec3& pos)
{
	// Round the precise float position to the nearest integer grid coordinate
	int x = static_cast<int>(round(pos.x));
	int y = static_cast<int>(round(pos.y));
	int z = static_cast<int>(round(pos.z));

	// Use central differences on the integer grid to approximate the gradient
	float nx = GetDensity(x + 1, y, z) - GetDensity(x - 1, y, z);
	float ny = GetDensity(x, y + 1, z) - GetDensity(x, y - 1, z);
	float nz = GetDensity(x, y, z + 1) - GetDensity(x, y, z - 1);

	// Normalize the resulting vector
	return glm::normalize(glm::vec3(nx, ny, nz));
}