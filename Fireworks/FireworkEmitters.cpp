#include "FireworkEmitters.h"
#include "Utility.h"

void RocketInitialiser::initialise(particle& p)
{
	p.lifeSpan = 100.0f;
	p.radius = 1.0f;

	p.velocity.x = (float)(std::rand() % 20 - 10) * 0.5;
	p.velocity.y = (float)(std::rand() % 20 - 10) * 0.5;
	p.velocity.z = 0.0f;
}


void RocketUpdater::update(particle& p, float dt)
{
	//reduce lifespan over time
	p.lifeSpan -= 20.0f * dt;
	p.velocity.x = movevar(p.velocity.x, 0.0f, 3.0f * dt);
	p.velocity.y = movevar(p.velocity.y, 0.0f, 3.0f * dt);
	p.velocity.z = movevar(p.velocity.z, 0.0f, 3.0f * dt);
}


void RocketFallInitialiser::initialise(particle& p)
{
	p.lifeSpan = 100.0f;
	p.radius = 1.0f;

	p.velocity.x = (float)(std::rand() % 20 - 10) * 0.5;
	p.velocity.y = (float)(std::rand() % 20 - 10) * 0.5;
	p.velocity.z = 0.0f;
}


void RocketFallUpdater::update(particle& p, float dt)
{
	//reduce lifespan over time
	p.lifeSpan -= 45.0f * dt;
	p.radius += dt;
	p.velocity.x = movevar(p.velocity.x, 0.0f, 3.0f * dt);
	p.velocity.y = movevar(p.velocity.y, 0.0f, 3.0f * dt);
	p.velocity.z = movevar(p.velocity.z, 0.0f, 3.0f * dt);
}


void RocketEndInitialiser::initialise(particle& p)
{
	p.lifeSpan = 100.0f;
	p.radius = 2.0f;


	p.direction.x = (float)(std::rand() % 200);
	p.direction.y = (float)(std::rand() % 200);
	float mag = 50.0f + (float)(rand() % 20);
	p.velocity.x = cosf(p.direction.x) * sinf(p.direction.y) * mag;
	p.velocity.y = sinf(p.direction.x) * sinf(p.direction.y) * mag;
	p.velocity.z = cosf(p.direction.y) * mag;
}



void RocketEndUpdater::update(particle& p, float dt)
{
	//reduce lifespan over time
	p.lifeSpan -= 20.0f * dt;
	p.radius -= dt;
}


void RocketEmitter::initialise()
{
	life = 10.0f;
	velocity = 400 + (float)(std::rand() % 200);
}

void RocketEmitter::update(float dt)
{
	for (int i = 0; i < 3; i++)
	{
		auto& p = GetFreeParticle();
		p.initialiser = &psInitialiser;
		p.updater = &psUpdater;
		p.pos = position;
		p.initialiser->initialise(p);
		p.lifeSpan = life;

		if (p.lifeSpan > 50.0f)
			p.lifeSpan = 50;

		p.sprite = sprite;
	}
	position.z += velocity * dt;
	velocity = movevar(velocity, 0.0f, 400.0f * dt);
	life = velocity;
}
void RocketEmitter::destroy()
{
	for (int i = 0; i < 600; i++)
	{
		auto& p = GetFreeParticle();
		p.initialiser = &psEndInitialiser;
		p.updater = &psEndUpdater;
		p.pos = position;
		p.initialiser->initialise(p);
		p.sprite = sprite;
	}
}


void RocketFallEmitter::initialise()
{
	life = 12.0f;
	velocity = 250 + (float)(std::rand() % 200);
	velY = (float)(std::rand() % 160 - 80);
}

void RocketFallEmitter::update(float dt)
{
	for (int i = 0; i < 6; i++)
	{
		auto& p = GetFreeParticle();
		p.initialiser = &psInitialiser;
		p.updater = &psUpdater;
		p.pos = position;
		p.initialiser->initialise(p);
		p.sprite = sprite;
	}
	position.z += velocity * dt;
	position.y += velY * dt;
	velocity = movevar(velocity, -60.0f, 300.0f * dt);
	life -= 10.0f * dt;
}

void RocketFallEmitter::destroy()
{}
