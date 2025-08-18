#include "ParticleSystem/ParticleSystem.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Util.h"
#include <iostream>

// Static member initialization
unsigned int  Particle::s_NextID = 0;

ParticleSystem::ParticleSystem(Mesh& mesh, unsigned int maxParticleCount)
	:m_mesh(mesh), m_MaxParticleCount(maxParticleCount)
{
	glGenBuffers(1, &m_VBO);

	unsigned int meshVAO = mesh.GetVAO();
	glBindVertexArray(meshVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * maxParticleCount, m_Particles.data(), GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, position));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, color));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, color));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
}


void ParticleSystem::Update(float dt)
{
	if (m_Particles.size() < m_MaxParticleCount)
	{
		Particle p;

		p.position = glm::vec3(0.0f, 0.0f, 0.0f);
		p.velocity = glm::vec3(RandomNumInRange(-10.0f, 10.0f), RandomNumInRange(-5.0f, 5.0f), RandomNumInRange(-10.0f, 10.0f));
		p.color = {1.0f, 0.5f, 0.2f, 1.0f};
		p.lifetime = 1500.0f; 

		m_Particles.push_back(p);
	}

	for (Particle& particle : m_Particles)
	{
		particle.lifetime -= dt;
		particle.position += particle.velocity * dt;
	}

		m_Particles.erase(
			std::remove_if(m_Particles.begin(), m_Particles.end(),
				[](const Particle& p) {
					return p.lifetime <= 0.0f;
				}
			),
			m_Particles.end()
		);
	

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * m_Particles.size(), m_Particles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}