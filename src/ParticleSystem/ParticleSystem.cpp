#include "ParticleSystem/ParticleSystem.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Util.h"
#include <cfloat>


// Static member initialization
unsigned int  Particle::s_NextID = 0;

ParticleSystem::ParticleSystem(Mesh& mesh, unsigned int maxParticleCount)
	:m_mesh(mesh), m_MaxParticleCount(maxParticleCount), m_Gravity(0.0f, -10.0f, 0.0f), timeInterval(0.0f)

{
	glGenBuffers(1, &m_VBO);

	unsigned int meshVAO = mesh.GetVAO();
	glBindVertexArray(meshVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * maxParticleCount, nullptr, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, currentPosition));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, color));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, 0, sizeof(Particle), (void*)offsetof(struct Particle, scale));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
	glDeleteBuffers(1, &m_VBO);
}


void ParticleSystem::Update(double dt)
{
	timeInterval += dt;

	if (timeInterval > 0.1f)
	{
		if (m_Particles.size() < m_MaxParticleCount)
		{
			Particle p;

			glm::vec3 initialVelocity = glm::vec3{(float)RandomNumInRange(-1, 1), (float)RandomNumInRange(-1, 1), (float)RandomNumInRange(-1, 1)};
			p.currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			p.oldPosition = p.currentPosition - initialVelocity * (float)dt;

			p.color = {1.0f, 0.5f, 0.2f, 1.0f};
			p.lifetime = 100.0f;
			p.scale = 0.3f;
			p.mass = 10.0f;

			m_Particles.push_back(p);
			timeInterval = 0.0f;
		}

	}

	for (Particle& particle : m_Particles)
	{
		// Decrease Lifetime
		particle.lifetime -= dt;

		// Update Particle location 
		particle.Accelerate(m_Gravity);
		particle.UpdatePosition(dt);
	}
	ApplyConstraints();

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

void ParticleSystem::ApplyConstraints()
{
	glm::vec3 spherePosition {0.0f, 0.0f, 0.0f,};
	const float sphereRadius = 2.0f;
	for (Particle& particle : m_Particles)
	{
		const glm::vec3 toParticle = particle.currentPosition - spherePosition;
		const float dist = glm::length(toParticle);
		const glm::vec3 vel = particle.currentPosition - particle.oldPosition;

		if (dist > sphereRadius - particle.scale)
		{
			const glm::vec3 norm = glm::normalize(toParticle);
			particle.currentPosition = spherePosition + norm * (sphereRadius - particle.scale);
			const glm::vec3 preCollisionVelocity = particle.currentPosition - particle.oldPosition;
			const glm::vec3 reflectedVelocity = glm::reflect(preCollisionVelocity, norm);
			particle.oldPosition = particle.currentPosition - reflectedVelocity;
		}
	}
}

float ParticleSystem::GetCurrentPosition()
{
	for (Particle& particle : m_Particles)
	{
		return particle.velocity.x;
	}
}



void ParticleSystem::Draw(Shader& shader)
{
	m_mesh.Draw(shader, m_MaxParticleCount);
}