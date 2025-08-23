#pragma once 

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <cstdint>


struct Particle
{
	Particle()
		:currentPosition(0.0f, 0.0f, 0.0f), oldPosition(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f),
		color(0.0f, 0.0f, 0.0f, 1.0f), lifetime(0.0f), scale(0.0f), id(0), mass(0.0f)
	{
		id = s_NextID++;
	}

	glm::vec3 currentPosition;
	glm::vec3 oldPosition;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec4 color;
	float mass;
	float lifetime;
	float scale;
	static unsigned int s_NextID;
	unsigned int id;

	bool operator==(const Particle& other) const
	{
		return id == other.id;
	}

	void UpdatePosition(float dt)
	{
		const glm::vec3 velocity = currentPosition - oldPosition;
		oldPosition = currentPosition;

		// Update Position using Vertlet integration
		currentPosition += velocity + acceleration * dt * dt;
		acceleration = {0.0f, 0.0f, 0.0f};
	}

	void Accelerate(glm::vec3 acc)
	{
		acceleration += acc;
	}
};

struct ParticleGPU
{
	glm::vec3 position;
	glm::vec4 color;
	float scale;
};


class ParticleSystem
{
public:
	ParticleSystem(Mesh& mesh, unsigned int maxParticleCount);
	~ParticleSystem();

	void Update(double dt);
	void Draw(Shader& shader);
	float GetCurrentPosition();
	int GetParticleCount();

	float timeInterval;

private:
	void ApplySphereConstraints(Particle& particle);
	void ApplyParticleCollisionBF();
	void ApplyParticleCollisionSP(Particle& obj);

	glm::ivec3 GetCellCoords(const glm::vec3& pos);
	int64_t GetHashCoords(const glm::ivec3& coords);

	std::vector<Particle> m_Particles;
	std::vector<ParticleGPU> m_ParticlesGPU;
	Mesh m_Mesh;
	unsigned int m_VBO;
	unsigned int m_MaxParticleCount;
	glm::vec3 m_Gravity;

	std::unordered_map<int64_t, std::vector<Particle*>> m_SpacialHash;
	float m_CellSize;
};