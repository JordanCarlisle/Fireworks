#include "ParticleSystem.h"

#include <algorithm>

ParticleSystem::ParticleSystem()
{
	memset(&pInstances, 0, sizeof(particle_instance) * MAX_PARTICLES);
	memset(&particles, 0, sizeof(particle) * MAX_PARTICLES);
}

ParticleSystem::~ParticleSystem()
{
	for (auto* e : emitters)
	{
		delete e;
	}
}

particle& AParticleEmitter::GetFreeParticle()
{
	return particleSystem->GetFreeParticle();
}

size_t ParticleSystem::GetNumParticles()
{
	return numParticles;
}

particle_instance* ParticleSystem::GetInstanceBuffer()
{
	return pInstances;
}


particle& ParticleSystem::GetFreeParticle()
{
	for (size_t i = lastParticle; i < MAX_PARTICLES; i++)
	{
		if (particles[i].lifeSpan <= 0.0f)
		{
			lastParticle = i;
			particles[i].pos.set(0.f, 0.f, 0.f);
			return particles[i];
		}
	}
	for (size_t i = 0; i < lastParticle; i++)
	{
		if (particles[i].lifeSpan <= 0.0f)
		{
			lastParticle = i;
			particles[i].pos.set(0.f, 0.f, 0.f);
			return particles[i];
		}
	}
	return particles[0];
}

void ParticleSystem::Update(float dt, v3 cameraPos)
{
	for (auto it = emitters.begin(); it != emitters.end(); )
	{
		if ((*it)->getlife() <= 0.0)
		{
			(*it)->destroy();
			delete (*it);
			it = emitters.erase(it);
		}
		else
		{
			(*it)->update(dt);
			++it;
		}
	}

	numParticles = 0;
	for (size_t i = 0; i < MAX_PARTICLES; i++)
	{
		particle& p = particles[i];
		if (p.lifeSpan > 0.0f) p.lifeSpan -= 10.0f * dt;
		if (p.lifeSpan > 0.0f)
		{
			//update particle
			if (p.updater != nullptr)
			{
				p.updater->update(p, dt);
			}

			//calculate distance from camera
			v3 particlePos = p.pos - cameraPos;

			p.camDistance = particlePos.length();

			p.pos += v3(
				p.velocity.x * dt,
				p.velocity.y * dt,
				p.velocity.z * dt);

			pInstances[i].position = p.pos;

			if (p.sprite == 0) pInstances[i].alpha.x = 0.0f;
			if (p.sprite == 1) pInstances[i].alpha.x = 1.0f;
			if (p.sprite == 2) pInstances[i].alpha.x = 2.0f;
			if (p.sprite == 3) pInstances[i].alpha.x = 3.0f;

			pInstances[i].alpha.w = p.lifeSpan * 0.01f;
			pInstances[i].scale.set(p.radius, p.radius);

			numParticles++;
		}
		else
		{
			pInstances[i].position.set(0.f, 0.f, 0.f);
			pInstances[i].alpha = 0.0f;
			pInstances[i].scale.set(0.0f, 0.0f);
			p.camDistance = -1.0f;
		}
	}

	//sort transparent particles by order of distance from the camera
	if (numParticles)
	{
		std::sort(&particles[0], &particles[MAX_PARTICLES], [](particle& a, particle& b) {
			return a.camDistance > b.camDistance;
		});
	}
}