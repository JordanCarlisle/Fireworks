#pragma once

#include "ParticleSystem.h"

class RocketInitialiser : public IParticleInitialiser
{
public:
	virtual void initialise(particle& p);
};

class RocketUpdater : public IParticleUpdater
{
public:
	virtual void update(particle& p, float dt);
};

class RocketFallInitialiser : public IParticleInitialiser
{
public:
	virtual void initialise(particle& p);
};

class RocketFallUpdater : public IParticleUpdater
{
public:
	virtual void update(particle& p, float dt);
};

class RocketEndInitialiser : public IParticleInitialiser
{
public:
	virtual void initialise(particle& p);
};



class RocketEndUpdater : public IParticleUpdater
{
public:
	virtual void update(particle& p, float dt);
};


class RocketEmitter : public AParticleEmitter
{
private:
	RocketInitialiser psInitialiser;
	RocketUpdater psUpdater;
	RocketEndInitialiser psEndInitialiser;
	RocketEndUpdater psEndUpdater;

public:
	RocketEmitter(ParticleSystem* system) : AParticleEmitter(system) {};
	virtual void initialise();
	virtual void update(float dt);
	virtual void destroy();
};

class RocketFallEmitter : public AParticleEmitter
{
private:
	RocketFallInitialiser psInitialiser;
	RocketFallUpdater psUpdater;
	float velY;

public:
	RocketFallEmitter(ParticleSystem* system) : AParticleEmitter(system) {};
	virtual void initialise();
	virtual void update(float dt);
	virtual void destroy();
};
