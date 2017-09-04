#pragma once
#include "../Building.h"


class House : public Building
{
public:

	virtual sf::Vector2u getSize() {return { 2, 2 }; }

	virtual size_t getHouseCapacity() { return 2; }

	// Called before tick every logic update
	// We apply housing capacity here
	virtual void pretick(float dt, Empire* owner) { owner->housing += getHouseCapacity(); }

	// Called every logic update
	virtual void tick(float dt, Empire* owner) {}

	House(Board* board) : Building(board)
	{
		setViewRadius(4);
		setTexRect(sf::IntRect(0, 3, 2, 2));
		setBuildingTexRect(sf::IntRect(2, 3, 2, 2));
		setColorTexRect(sf::IntRect(4, 3, 2, 2));
		setMaxHealth(100.0f);
	}
	~House() {}
};

