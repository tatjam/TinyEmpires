#pragma once
#include "Building.h"
#include "GameState.h"
#include "Entity.h"
#include <thread>


class Building;
class GameState;
class Entity;

class Empire
{
private:

	std::thread* viewThread;
	bool viewThreadRun;

	bool viewJustFinishing = false;

	float itimer = 0.0f;

	bool needsReload = false;

public:

	sf::Color color = sf::Color::White;

	GameState* game;

	// Not using std::vector<bool> because of its speed
	std::vector<uint8_t> view;

	// View to use, use this as view is usually being used by the
	// worker thread
	std::vector<uint8_t> viewFinal;

	bool viewReady = false;

	sf::Image rendered;
	sf::Image renderedBuffer;
	sf::Texture renderedT;
	sf::Texture prevRenderedT;


	std::vector<Building*> buildings;
	std::vector<Entity*> entities;

	// Resources
	size_t tech;
	size_t metal;
	size_t wood;
	size_t food;
	size_t rock;
	size_t housing;

	void update(float dt);
	void draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide);

	void renderWholeView();
	void updateView();
	void drawView(sf::RenderTarget* target);


	void launchViewThread();

	void end();

	Empire(GameState* gm);
	~Empire();
};


void viewRendererWorker(Empire* empire, bool* run);

