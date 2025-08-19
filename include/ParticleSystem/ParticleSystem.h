#pragma once 

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include <iostream>


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


class ParticleSystem
{
public:
	ParticleSystem(Mesh& mesh, unsigned int maxParticleCount);
	~ParticleSystem();

	void Update(double dt);
	void UpdatePosition(float dt);
	void Draw(Shader& shader);
	void ApplyConstraints();
	float GetCurrentPosition();

	float timeInterval;

private:
	std::vector<Particle> m_Particles;
	Mesh m_mesh;
	unsigned int m_VBO;
	unsigned int m_MaxParticleCount;
	glm::vec3 m_Gravity;

};