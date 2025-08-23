#include "ParticleSystem/ParticleSystem.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Util.h"
#include <cfloat>


// Static member initialization
unsigned int  Particle::s_NextID = 0;

ParticleSystem::ParticleSystem(Mesh& mesh, unsigned int maxParticleCount)
	:m_Mesh(mesh), m_MaxParticleCount(maxParticleCount), m_Gravity(0.0f, -10.0f, 0.0f), timeInterval(0.0f), m_CellSize(0.2), 
	m_ParticlesGPU(0.0f)
{
	m_ParticlesGPU.resize(m_MaxParticleCount);
	m_SpacialHash.reserve(maxParticleCount);

	glGenBuffers(1, &m_VBO);

	unsigned int meshVAO = mesh.GetVAO();
	glBindVertexArray(meshVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleGPU) * maxParticleCount, nullptr, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(ParticleGPU), (void*)offsetof(struct ParticleGPU, position));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, 0, sizeof(ParticleGPU), (void*)offsetof(struct ParticleGPU, color));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, 0, sizeof(ParticleGPU), (void*)offsetof(struct ParticleGPU, scale));

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

	if (timeInterval > 0.0001f)
	{
		if (m_Particles.size() < m_MaxParticleCount)
		{
			Particle p;

			glm::vec3 initialVelocity = glm::vec3{RNG::Get().RandomFloatInRange(-5.0, 5.0), RNG::Get().RandomFloatInRange(-5.0, 5.0), RNG::Get().RandomFloatInRange(-5.0, 5.0)};
			p.currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			p.oldPosition = p.currentPosition - initialVelocity * (float)dt;

			p.color = {1.0f, 0.5f, 0.2f, 1.0f};
			p.lifetime = 100.0f;
			p.scale = 0.1f;
			p.mass = 10.0f;

			m_Particles.push_back(p);
			timeInterval = 0.0f;
		}
	}

	// Simulation Substeps
	unsigned int substeps = 2;
	double sub_dt = dt / substeps;
	for (int i = 0; i < substeps; i++)
	{
		for (Particle& particle : m_Particles)
		{
			// Decrease Lifetime
			particle.lifetime -= sub_dt;

			// Update Particle location 
			particle.Accelerate(m_Gravity);
			particle.UpdatePosition(sub_dt);
			ApplySphereConstraints(particle);
		}

		m_SpacialHash.clear();
		for (Particle& particle : m_Particles)
		{
			glm::ivec3 cellCoords = GetCellCoords(particle.currentPosition);
			int64_t key = GetHashCoords(cellCoords);

			auto& bucket = m_SpacialHash[key];
			if (bucket.empty())
			{
				bucket.reserve(16);
			}
			bucket.push_back(&particle);
		}

		//ApplyParticleCollisionBF
		for (Particle& particle : m_Particles)
		{
			ApplyParticleCollisionSP(particle);
		}
	}

	m_Particles.erase(
		std::remove_if(m_Particles.begin(), m_Particles.end(),
			[](const Particle& p) {
				return p.lifetime <= 0.0f;
			}
		),
		m_Particles.end()
	);

	
	for (int i = 0; i < m_Particles.size(); ++i)
	{
		m_ParticlesGPU[i].position = m_Particles[i].currentPosition;
		m_ParticlesGPU[i].color = m_Particles[i].color;
		m_ParticlesGPU[i].scale = m_Particles[i].scale;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleGPU) * m_ParticlesGPU.size(), m_ParticlesGPU.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::ApplySphereConstraints(Particle& particle)
{
	glm::vec3 spherePosition {0.0f, 0.0f, 0.0f,};
	const float sphereRadius = 8.0f;
		const glm::vec3 toParticle = particle.currentPosition - spherePosition;
		const float distSqrt = (toParticle.x * toParticle.x) + (toParticle.y * toParticle.y) + (toParticle.z * toParticle.z);
		const glm::vec3 vel = particle.currentPosition - particle.oldPosition;

		const float comparisonRadius = sphereRadius - particle.scale;
		if (distSqrt > comparisonRadius * comparisonRadius)
		{
			float dist = glm::length(toParticle);
			const glm::vec3 norm = toParticle / dist;
			particle.currentPosition = spherePosition + norm * (sphereRadius - particle.scale);
			const glm::vec3 preCollisionVelocity = particle.currentPosition - particle.oldPosition;
			const glm::vec3 reflectedVelocity = glm::reflect(preCollisionVelocity, norm);
			particle.oldPosition = particle.currentPosition - reflectedVelocity;
		}
	}


void ParticleSystem::ApplyParticleCollisionBF()
{
	for (int i = 0; i < m_Particles.size(); ++i)
	{
		Particle& obj1 = m_Particles[i];
		for (int j = i + 1; j < m_Particles.size(); ++j)
		{
			Particle& obj2 = m_Particles[j];
			glm::vec3 direction = obj2.currentPosition - obj1.currentPosition;
			float dist = glm::length(direction);
			if (dist < obj1.scale + obj2.scale)
			{
				glm::vec3 norm = glm::normalize(direction);
				float delta = (obj1.scale + obj2.scale) - dist;
				obj1.currentPosition -= norm * 0.5f * delta;
				obj2.currentPosition += norm * 0.5f * delta;
			}
		}
	}
}

// Spatial Partitioning function using Hashing
void ParticleSystem::ApplyParticleCollisionSP(Particle& obj)
{
	glm::ivec3 cellCoords = GetCellCoords(obj.currentPosition);
	for (int i = cellCoords.x - 1; i <= cellCoords.x + 1; i++)
	{
		for (int j = cellCoords.y - 1; j <= cellCoords.y + 1; j++)
		{
			for (int k = cellCoords.z - 1; k <= cellCoords.z + 1; k++)
			{
				auto got = m_SpacialHash.find(GetHashCoords(glm::ivec3(i, j, k)));
				if (got != m_SpacialHash.end())
				{
					for (Particle* neighborParticle : got->second)
					{
						if (&obj == neighborParticle) continue;

						if (obj.id < neighborParticle->id)
						{
							glm::vec3 collisionAxis = neighborParticle->currentPosition - obj.currentPosition;
							float dist = glm::length(collisionAxis);
							
							if (dist > 0.0f && dist < obj.scale + neighborParticle->scale)
							{
								glm::vec3 norm = glm::normalize(collisionAxis);
								float delta = (obj.scale + neighborParticle->scale) - dist;
								obj.currentPosition -= norm * 0.5f * delta;
								neighborParticle->currentPosition += norm * 0.5f * delta;
							}
						}
					}
				}
			}
		}
	}
}

inline glm::ivec3 ParticleSystem::GetCellCoords(const glm::vec3& pos)
{

	return glm::ivec3(glm::floor(pos / m_CellSize));
}

inline int64_t ParticleSystem::GetHashCoords(const glm::ivec3& coords)
{
	constexpr int64_t P1 = 73856093LL;
	constexpr int64_t P2 = 19349669LL; 
	constexpr int64_t P3 = 83492791LL;
	return (coords.x * P1) ^ (coords.y * P2) ^ (coords.z * P3);
}

float ParticleSystem::GetCurrentPosition()
{
	for (Particle& particle : m_Particles)
	{
		return particle.velocity.x;
	}
}

int ParticleSystem::GetParticleCount()	
{
	return m_Particles.size();
}

void ParticleSystem::Draw(Shader& shader)
{
	m_Mesh.Draw(shader, (unsigned)m_Particles.size());
}