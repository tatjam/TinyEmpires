#pragma once
#include "../Entity.h"

// When mining workers follow this order by default
// 1 - Mine 
// 2 - Haul to closest location
// 3 - Find new mining objective of same type (within radius)
// (If none are found the worker becomes idle)
//
// Effectively the worker gives orders to itself!

class Worker : public Entity
{
private:

	Order current;
	bool reached = false;
	bool inOrder = false;

	float animTimer = 0.0f;

	// 0 - No Anim
	// 1 - Build Anim
	// 2 - Destroy Anim
	// 3 - Axe Anim
	// 4 - Pickaxe Anim
	size_t inAnim = 0;

public:

	virtual float getBuildPower() { return 5.0f; }
	virtual float getDestroyPower() { return 2.5f; }
	virtual float getMiningPower() { return 50.0f; }

	virtual void pretick(float dt) {}

	virtual void tick(float dt);

	//virtual void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide) {}

	virtual void postDraw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	// Called when the entity is spawned
	virtual void start() {}

	// Called when the entity is killed
	virtual void end() {}

	Worker(Empire* owner) : Entity(owner)
	{
		setViewRadius(6);
		setSpeed(5.0f);
		setTexPos({ 0, 5 });
		setColorTexPos({ 0, 6 });
		setMultipliers(1.0f, 0.7f, 0.2f, 0.0f, 1.3f);
		generatePathCosts();
	}

	~Worker();
};
