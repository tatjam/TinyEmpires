#pragma once
#include "Board.h"
#include "../Other/SettingsManager.h"

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



class GameState
{
public:

	SettingsManager* sets;

	std::vector<AIPlayer> ai;

	std::vector<Empire> empires;

	Player player;

	Board* board;

	void update(float dt, sf::RenderWindow* win);

	void start(sf::Texture* spriteSheet, size_t spriteWidth);
	void rerender();

	void draw(sf::RenderTarget* target);

	bool getAnyKeyDown(std::vector<Key>* keys);

	GameState(SettingsManager* sets);
	~GameState();
};

