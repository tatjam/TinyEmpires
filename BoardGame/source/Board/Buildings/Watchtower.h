#pragma once
#include "../Building.h"


class Watchtower : public Building
{
public:

	sf::Sprite sprite;

	virtual sf::Vector2u getSize() { return { 2, 3 }; };

	// Called before tick every logic update
	// We apply housing capacity here
	virtual void pretick(float dt, Empire* owner) {};

	// Called every logic update
	virtual void tick(float dt, Empire* owner) {};

	Watchtower(Board* board) : Building(board)
	{
		setViewRadius(16);
		setTexRect(sf::IntRect(3, 3, 2, 3));
	}

	~Watchtower() {}
};

