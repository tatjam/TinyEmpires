#pragma once
#include "Board.h"
#include "../Other/SettingsManager.h"
#include "Entity.h"
#include "Building.h"


class Empire;
class Board;

struct Player
{
	// Position of the camera
	sf::Vector2f cameraPos = { 0.0f, 0.0f };
	float cameraZoom = 0.5f;
	Empire* assigned;
};

struct AIPlayer
{
	std::string name;
	uint16_t id;
	Empire* assigned;
};


class Entity;
class Building;
struct Order;

class GameState
{
private:

	float maptimer = 0.0f;

	sf::Vector2f clickPos;
	sf::Vector2f newPos;
	bool inSelection = false;

	std::vector<Entity*> selected;
	std::vector<Building*> selectedBuildings;

	bool entitiesOnly = false;

	float animProgress = 0.0f;

	bool inAnim = false;

	// 0 - Move
	// 1 - Build
	// 2 - Attack
	// 3 - Mine
	size_t animType = 0;

	sf::Vector2i animTarget;

	bool held = false;

	std::vector<std::pair<Entity*, Order>> ordersBeingGiven;

	float otimer = 0.0f;

	bool inMine = false;

public:

	SettingsManager* sets;

	std::vector<AIPlayer> ai;

	std::vector<Empire> empires;

	sf::Image minimap;
	sf::Texture minimapT;

	Player player;

	Board* board;

	void update(float dt, sf::RenderWindow* win);

	void userUpdate(float dt, sf::RenderWindow* win);

	void cameraUpdate(float dt, sf::RenderWindow* win);

	void renderMinimap();

	void start(sf::Texture* spriteSheet, size_t spriteWidth);
	void rerender();

	void draw(sf::RenderTarget* target);
	void drawUI(sf::RenderTarget* target);

	bool getAnyKeyDown(std::vector<Key>* keys);

	void addEmpire();

	GameState(SettingsManager* sets);
	~GameState();
};

