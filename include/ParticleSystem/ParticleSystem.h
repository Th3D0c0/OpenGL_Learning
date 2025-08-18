#pragma once 

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"


struct Particle
{
	Particle()
	{
		id = s_NextID++;
	}

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float lifetime;
	float scale;
	static unsigned int s_NextID;
	float id;

	bool operator==(const Particle& other) const
	{
		return id == other.id;
	}
};


class ParticleSystem
{
public:
	ParticleSystem(Mesh& mesh, unsigned int maxParticleCount);
	~ParticleSystem();

	void Update(float dt);


private:
	std::vector<Particle> m_Particles;
	Mesh m_mesh;
	unsigned int m_VBO;
	unsigned int m_MaxParticleCount;
};