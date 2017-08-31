#pragma once
#include "Empire.h"
#include <thread>

class Empire;

struct PathCosts;

enum OrderType
{
	NO_ORDER,
	MOVE,
	ATTACK,
	BUILD,
	MINE
};

struct Order
{
	OrderType type = NO_ORDER;
	sf::Vector2i target;
};

class Entity
{
private:

	PathCosts* generatedCosts;

	Empire* owner;

	sf::Vector2i position;

	// Offset from 0-1 in tileSide range
	sf::Vector2f offset;

	sf::Vector2u texPos;

	size_t viewRadius;

	sf::Sprite sprite;

	int health;
	int maxHealth;

	// Speed in tiles per second
	float speed = 1.0f;
	bool inMovement = false;
	sf::Vector2i movTarget = { 0, 0 };
	float movProgress = 0.0f;

	float pathMult = 2.0f;
	float groundMult = 1.0f;
	float treeMult = 0.5f;
	float shallowMult = 0.1f;
	float deepMult = 0.0f;
	bool flyer = false;

	float rspeed = 0.0f;

	std::vector<sf::Vector2u> activePath;
	bool inPath;
	size_t pathptr;
	sf::Vector2u wantedTarget;

	bool threadWorkerFinished = false;
	bool threadWorking = false;

	std::thread* worker;

	Order activeOrder = Order();

public:

	virtual sf::Vector2i getPosition();
	virtual void setPosition(sf::Vector2i nPos);

	virtual void setTexPos(sf::Vector2u pos);
	virtual sf::Vector2u getTexPos();

	virtual sf::IntRect getTextureRect(size_t tileSide);

	virtual size_t getViewRadius();
	virtual void setViewRadius(size_t r);

	virtual void fillView();

	virtual void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	virtual void pretick(float dt){}
	virtual void tick(float dt) {}

	// Called when the entity is spawned
	virtual void start() {}

	// Called when the entity is killed
	virtual void end() {}

	// damage can be used to heal too!
	virtual void damage(int damage);

	virtual void giveOrder(OrderType order, sf::Vector2i target);

	Order getActiveOrder();
	bool hasOrder();

	// Called automatically, handles internal movement
	void updateMovement(float dt);

	// Called automatically, handles internal pathing
	void updatePathing(float dt);

	// Sets target for internal movement handler
	virtual float setMovementTarget(sf::Vector2i tile);

	// Sets target for internal movement handler
	// Takes offset from current position instead
	// of target
	virtual float setMovementOffset(sf::Vector2i offset);

	// Returns true if ready to do another movement
	virtual bool getMovementStatus();

	// Uses pathfinder to generate path to location
	// and activates it
	// If threaded is set to true it will work in a
	// thread
	void moveTo(sf::Vector2i target, bool threaded = false);

	void startPath(std::vector<sf::Vector2u> p);
	void cancelPath();

	// Returns true if currently on a path
	bool isOnPath();
	std::vector<sf::Vector2u>* getPath();

	virtual float getSpeed();
	virtual void setSpeed(float speed);

	float getGroundMultiplier() { return groundMult; }
	float getTreeMultiplier() { return treeMult; }
	float getShallowMultiplier() { return shallowMult; }
	float getDeepMultiplier() { return deepMult; }
	bool isFlyer() { return flyer; }
	void setFlyer(bool val){ flyer = val;}

	void setMultipliers(float ground, float tree, float shallow, float deep, float path)
	{
		groundMult = ground; treeMult = tree; shallowMult = shallow; deepMult = deep; pathMult = path;
	}

	// Helper function for movement using floats
	// Interpolates between two positions
	// Time ranges from 0 to 1
	// Sets offset (and returns it)
	// Target is a vector around (0, 0)
	sf::Vector2f interpTo(sf::Vector2i target, float time);

	void setOffset(sf::Vector2f of);
	sf::Vector2f getOffset();

	int getHealth();
	int getMaxHealth();
	void setHealth(int h);
	void setMaxHealth(int h, bool resetHealth);

	PathCosts getPathCosts();
	void generatePathCosts();

	void setOwner(Empire* owner);
	Empire* getOwner();

	Entity(Empire* owner);
	~Entity();
};

// Threaded path computing
void computePath(Empire* owner, std::vector<sf::Vector2u>* out, 
	sf::Vector2u from, sf::Vector2u to, PathCosts costs, bool* finished);