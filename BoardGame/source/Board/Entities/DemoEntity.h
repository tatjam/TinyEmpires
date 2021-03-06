#pragma once
#include "../Entity.h"

class DemoEntity : public Entity
{
private:

	sf::RenderWindow* win;
	size_t tileSide;

	bool onPath = false;
	size_t pathPtr = 0;
	Path path;

	sf::Vector2u wantedTarget;

public:



	virtual void pretick(float dt);

	virtual void tick(float dt);

	// Called when the entity is spawned
	virtual void start();

	// Called when the entity is killed
	virtual void end();

	virtual void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	DemoEntity(Empire* owner) : Entity(owner)
	{
		setViewRadius(6);
		setSpeed(5.0f);
		setTexPos({ 0, 5 });
		setMultipliers(1.0f, 0.7f, 0.2f, 0.0f, 1.3f);
		generatePathCosts();
	}

	~DemoEntity();
};

