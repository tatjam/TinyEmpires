#pragma once
#include "../Building.h"


class Windmill : public Building
{
private:

	float wingRotation = 0.0f;

public:

	virtual sf::Vector2u getSize() { return { 2, 3 }; }

	// Called every logic update
	virtual void tick(float dt, Empire* owner);

	virtual void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	Windmill(Board* board) : Building(board)
	{
		setViewRadius(3);
		setTexRect(sf::IntRect(9, 3, 2, 3));
		setBuildingTexRect(sf::IntRect(14, 3, 2, 3));
		setMaxHealth(300.0f);
	}
	~Windmill() {}
};

