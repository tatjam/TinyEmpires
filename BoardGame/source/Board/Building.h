#pragma once

#include "SFML/Graphics.hpp"
#include "Empire.h"
#include "Board.h"

class Empire;
class Board;


class Building
{
private:

	sf::Vector2i pos;
	sf::IntRect texRect;
	sf::IntRect buildingTexRect;
	sf::IntRect colorTexRect;

	Board* attached;

	size_t viewRadius;

	sf::Sprite sprite;

	float health = 0.0f;
	float maxHealth = 0.0f;

	float fireTimer = 0.0f;

	size_t fireFrame = 0;

	// Must reach health
	float buildProgress = 0.0f;

	size_t id = 0;

	Empire* owner;

	bool selected;
	Empire* selector;

public:

	virtual sf::Vector2i getPosition();
	virtual void setPosition(sf::Vector2i nPos);
	virtual sf::Vector2u getSize() = 0;

	// Called before tick every logic update
	virtual void pretick(float dt, Empire* owner) {};

	bool isBuilt() { return buildProgress >= maxHealth; }

	void build(float increase);
	float getBuildProgress() {return buildProgress;}

	virtual void select(Empire* selector);
	virtual void unselect(Empire* selector);

	virtual bool isSelected() { return selected; }

	virtual void setTexRect(sf::IntRect nRect);
	virtual void setBuildingTexRect(sf::IntRect nRect);
	virtual void setColorTexRect(sf::IntRect nRect);

	virtual sf::IntRect getTexRect();
	virtual sf::IntRect getFinalRect(size_t tileSide);

	virtual sf::IntRect getBuildingTexRect();
	virtual sf::IntRect getBuildingFinalRect(size_t tileSide);

	virtual sf::IntRect getColorTexRect();
	virtual sf::IntRect getColorFinalRect(size_t tileSide);

	virtual size_t getViewRadius();
	virtual void setViewRadius(size_t i);

	// Applies view radius of the building
	// Should be called every time a view update
	// is required
	virtual void fillView(Empire* owner);

	// Called every logic update
	virtual void tick(float dt, Empire* owner) {};

	// Internal update used for animations and such
	void baseUpdate(float dt, Empire* owner); 

	virtual Empire* getOwner() { return this->owner; }

	// Called every frame (video frame)
	// ONLY IF we are visible
	virtual void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	// Internal, draws damage effects
	void drawDamage(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	// Called when the building is built (after it has been set to
	// correct location!)
	// Adds itself to the ground tiles. Make sure correct rect is used
	virtual void start();

	// Called when the building is destroyed
	// Removes itself from ground tiles
	void end();

	// Called when the building is destroyed
	virtual void die();

	virtual void setMaxHealth(float val, bool resetHealth = true);
	virtual float getMaxHealth() { return maxHealth; }

	virtual void damage(float damage);
	virtual float getHealth() { return health; };
	virtual void setHealth(float val);

	Building(Board* board) { attached = board; id = rand(); }
	~Building() { end(); }
};