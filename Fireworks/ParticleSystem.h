#pragma once
#include "v2.h"
#include "v4.h"
#include <vector>

#define MAX_PARTICLES 65536

class ParticleSystem;
class IParticleInitialiser;
class IParticleUpdater;

//data structure for a single particle
struct particle
{
	float radius;
	float lifeSpan;
	float fadeFactor;
	v3 pos;
	v3 velocity;
	v3 direction;
	float camDistance;
	int sprite;
	IParticleInitialiser* initialiser;
	IParticleUpdater* updater;
};

//used by the instance buffer
struct particle_instance
{
	v3 position;
	v4 alpha;
	v2 scale;
	int sprite[2];
};

//Interface for intialising particles
class IParticleInitialiser
{
public:
	virtual void initialise(particle& p) = 0;
};

//Interface for updating particles
class IParticleUpdater
{
public:
	virtual void update(particle& p, float dt) = 0;
};

//Abstract emitter class for creating particle emitters
class ParticleSystem;
class AParticleEmitter
{
private:
	ParticleSystem* particleSystem;
protected:
	v3 position;
	float velocity;
	v3 angle;
	float life;
	int sprite;
public:
	AParticleEmitter(ParticleSystem* system) : particleSystem(system)
	{}
	virtual void initialise() = 0;
	virtual void update(float dt) = 0;
	virtual void destroy() = 0;
	particle& GetFreeParticle();
	float getlife() { return life; }
	void setpos(v3 ps, int spr) { position = ps; sprite = spr; }
};

class ParticleSystem
{
private:

	particle particles[MAX_PARTICLES];
	particle_instance pInstances[MAX_PARTICLES];

	size_t lastParticle;
	size_t numParticles;

	std::vector<AParticleEmitter*> emitters;

public:

	ParticleSystem();
	~ParticleSystem();

	particle_instance* GetInstanceBuffer();
	size_t GetNumParticles();
	void Update(float dt, v3 cameraPos);

	particle& GetFreeParticle();

	template<typename T>
	void CreateEmitter(v3 pos, int sprite)
	{
		AParticleEmitter* e = new T(this);
		e->setpos(pos, sprite);
		e->initialise();

		emitters.push_back(e);
	}
};