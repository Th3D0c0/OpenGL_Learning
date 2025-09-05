#include "PlanetGenerator/PlanetGen.h"
#include"PlanetGenerator/Tables.h"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <unordered_map>
#include <algorithm>

Planet::Planet()
{
	SetupMesh();
	m_Noise00.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	m_Noise00.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_Noise00.SetFrequency(0.08f);
	m_Noise00.SetFractalOctaves(4);
	m_Noise00.SetFractalLacunarity(2.0f);
	m_Noise00.SetFractalGain(0.5);


	m_Noise01.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	m_Noise01.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
	m_Noise01.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);
	m_Noise01.SetFrequency(0.01f);
	m_Noise00.SetFrequency(0.05);
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

	std::unordered_map<uint64_t, unsigned int> vertexMap;
	int nrOfVert = 0;

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

				uint8_t cubeIndex = CalculateCubeIndex(cornerDensities, -0.05f);

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
							glm::vec3 pGrid = InterpolateVertex(
								cornerPositions[cornerAIndex],
								cornerPositions[cornerBIndex],
								cornerDensities[cornerAIndex],
								cornerDensities[cornerBIndex],
								-0.05f);

							// 2) Convert GRID → NORMALIZED [-1,1] → WORLD (size = radius)
							glm::vec3 pNorm = (pGrid / (float)(m_CurrentResolution - 1) - 0.5f) * 2.0f;

							glm::vec3 pWorld = pNorm * radius;

							// 3) Compute normal from the density grid (still in grid space)
							glm::vec3 n = CalculateNormal(pGrid);

							// 4) Store
							Vertex v;
							v.Position = pWorld;
							v.Normal = n;
							m_Vertices.push_back(v);
							nrOfVert++;

							unsigned int newIndex = m_Vertices.size() - 1;
							vertexMap[edgeID] = newIndex;
							triIndices[j] = newIndex;
						}
						else
						{
							triIndices[j] = vertexMap[edgeID];
						}
					}
					if (triIndices[0] != triIndices[1] &&
						triIndices[1] != triIndices[2] &&
						triIndices[2] != triIndices[0])
					{
						m_Indices.push_back(triIndices[0]);
						m_Indices.push_back(triIndices[1]);
						m_Indices.push_back(triIndices[2]);
					}
				}
			}
		}
	}
	printf("Verteces: %d\n", nrOfVert);
	UpdateMeshBuffers();
}

void Planet::DrawPlanet(glm::mat4 viewMat4, glm::mat4 projMat4, const glm::vec3& lightPos, const glm::vec3& viewPos)
{
	m_Shader->use();
	m_Shader->setUniformValue("projection", projMat4);
	m_Shader->setUniformValue("view", viewMat4);
	m_Shader->setUniformValue("model", m_Transform.GetModelMatrix());

	m_Shader->setUniformValue("lightPos", lightPos);
	m_Shader->setUniformValue("viewPos", viewPos);
	m_Shader->setUniformValue("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));



	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
	m_CurrentResolution = resolution + 1;
	m_Radius = radius;

	std::vector<float> outDensities((resolution + 1) * (resolution + 1) * (resolution + 1));

	float gridPadding = 2.0f;

	float noiseScale1 = 1.7f;
	float noiseScale2 = 2.0f;
	float noiseScale3 = 5.0f;

	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			for (int z = 0; z < resolution; z++)
			{
				float currX = (x / (float)resolution - 0.5f) * 2.0f * radius * gridPadding;
				float currY = (y / (float)resolution - 0.5f) * 2.0f * radius * gridPadding;
				float currZ = (z / (float)resolution - 0.5f) * 2.0f * radius * gridPadding;

				glm::vec3 worldPos(currX, currY, currZ);

				float distToCenter = glm::length(worldPos);
				float density = distToCenter - radius;

				density += m_Noise00.GetNoise(
					(float)x,
					(float)y,
					(float)z) * noiseScale1;
				density += m_Noise01.GetNoise(
					(float)x,
					(float)y,
					(float)z) * noiseScale2;
				//density += m_Noise02.GetNoise(
				//	(float)x,
				//	(float)y,
				//	(float)z) * noiseScale3;

				int gridRes = resolution + 1;
				int index = x + y * gridRes + z * gridRes * gridRes;
				outDensities[index] = density;
			}
		}
	}
	return outDensities;
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
	t = glm::clamp(t, 0.001f, 0.999f);
	return p1 + t * (p2 - p1);
}

// trilinear sample of the density field at fractional grid coords
float Planet::GetDensityTrilinear(float x, float y, float z)
{
	if (m_DensityValues.empty() || m_CurrentResolution == 0) return 0.0f;

	const float maxIdx = (float)m_CurrentResolution - 1.0f;
	x = glm::clamp(x, 0.0f, maxIdx);
	y = glm::clamp(y, 0.0f, maxIdx);
	z = glm::clamp(z, 0.0f, maxIdx);

	int x0 = (int)floor(x);
	int y0 = (int)floor(y);
	int z0 = (int)floor(z);
	int x1 = std::min(x0 + 1, m_CurrentResolution - 1);
	int y1 = std::min(y0 + 1, m_CurrentResolution - 1);
	int z1 = std::min(z0 + 1, m_CurrentResolution - 1);

	float xd = x - x0;
	float yd = y - y0;
	float zd = z - z0;

	auto D = [&](int xi, int yi, int zi)->float {
		unsigned int idx = xi + yi * m_CurrentResolution + zi * m_CurrentResolution * m_CurrentResolution;
		return m_DensityValues[idx];
		};

	float c000 = D(x0, y0, z0);
	float c100 = D(x1, y0, z0);
	float c010 = D(x0, y1, z0);
	float c110 = D(x1, y1, z0);
	float c001 = D(x0, y0, z1);
	float c101 = D(x1, y0, z1);
	float c011 = D(x0, y1, z1);
	float c111 = D(x1, y1, z1);

	float c00 = c000 * (1.0f - xd) + c100 * xd;
	float c01 = c001 * (1.0f - xd) + c101 * xd;
	float c10 = c010 * (1.0f - xd) + c110 * xd;
	float c11 = c011 * (1.0f - xd) + c111 * xd;

	float c0 = c00 * (1.0f - yd) + c10 * yd;
	float c1 = c01 * (1.0f - yd) + c11 * yd;

	return c0 * (1.0f - zd) + c1 * zd;
}

glm::vec3 Planet::CalculateNormal(const glm::vec3& posGrid)
{
	// central difference in grid-space using trilinear sampling
	const float eps = 1.0f; // one grid unit; you can reduce to 0.5 for softer normals
	float nx = GetDensityTrilinear(posGrid.x + eps, posGrid.y, posGrid.z) - GetDensityTrilinear(posGrid.x - eps, posGrid.y, posGrid.z);
	float ny = GetDensityTrilinear(posGrid.x, posGrid.y + eps, posGrid.z) - GetDensityTrilinear(posGrid.x, posGrid.y - eps, posGrid.z);
	float nz = GetDensityTrilinear(posGrid.x, posGrid.y, posGrid.z + eps) - GetDensityTrilinear(posGrid.x, posGrid.y, posGrid.z - eps);

	glm::vec3 n(nx, ny, nz);
	float len = glm::length(n);
	if (len == 0.0f) return glm::vec3(0.0f, 1.0f, 0.0f); // fallback normal
	return glm::normalize(n);
}

void Planet::SetLocation(const glm::vec3& location)
{
	m_Transform.SetLocation(location);
}

void Planet::SetRotation(const glm::vec3& rotation)
{
	m_Transform.SetRotation(rotation);
}

void Planet::SetScale(const glm::vec3& scale)
{
	m_Transform.SetScale(scale);
}

void Planet::SetNoiseFrequency(float frequency1, float frequency2, float frequency3)
{
	m_Noise00.SetFrequency(frequency1);
	m_Noise01.SetFrequency(frequency2);
	m_Noise02.SetFrequency(frequency3);
	LoadMesh(m_Radius, m_CurrentResolution);
}
