#include "GameState.h"



void GameState::update(float dt, sf::RenderWindow* win)
{
	// Update empires
	for (size_t i = 0; i < empires.size(); i++)
	{
		empires[i].update(dt);
	}

	// Handle camera movement
	
	if (getAnyKeyDown(&sets->cSets.incZoomKeys))
	{
		player.cameraZoom += *sets->cSets.cameraZoomSpeed * dt;
	}

	if (getAnyKeyDown(&sets->cSets.decZoomKeys))
	{
		player.cameraZoom -= *sets->cSets.cameraZoomSpeed * dt;
	}

	float speed = sets->cSets.scrollSpeed[0];
	if (getAnyKeyDown(&sets->cSets.speedKeys))
	{
		speed = sets->cSets.scrollSpeed[1];
	}

	sf::Vector2i mPos = sf::Mouse::getPosition(*win);
	bool inWindow = false;
	if (mPos.x > 0 && mPos.x < win->getSize().x && mPos.y > 0 && mPos.y < win->getSize().y)
	{
		inWindow = true;
	}

	if ((inWindow || sets->cSets.offscreenScroll) && sets->cSets.allowMouseScroll)
	{
		int minX = (int)(sets->cSets.triggers[0] * win->getSize().x);
		int maxX = (int)(sets->cSets.triggers[1] * win->getSize().x);
		int minY = (int)(sets->cSets.triggers[2] * win->getSize().y);
		int maxY = (int)(sets->cSets.triggers[3] * win->getSize().y);

		// Edges
		if (mPos.x < minX) { player.cameraPos.x -= speed * dt; }
		if (mPos.x > maxX) { player.cameraPos.x += speed * dt; }
		if (mPos.y < minY) { player.cameraPos.y -= speed * dt; }
		if (mPos.y > maxY) { player.cameraPos.y += speed * dt; }
	}

	if (getAnyKeyDown(&sets->cSets.upKeys)) { player.cameraPos.y -= speed * dt; }
	if (getAnyKeyDown(&sets->cSets.rightKeys)) { player.cameraPos.x += speed * dt; }
	if (getAnyKeyDown(&sets->cSets.downKeys)) { player.cameraPos.y += speed * dt; }
	if (getAnyKeyDown(&sets->cSets.leftKeys)) { player.cameraPos.x -= speed * dt; }


}

void GameState::start(sf::Texture* spriteSheet, size_t spriteWidth)
{
	board = new Board(5, 5, spriteSheet, spriteWidth);
	board->generate(GenSettings(54321, -1.0f));
}

void GameState::rerender()
{
	board->render();
}

void GameState::draw(sf::RenderTarget* target)
{
	sf::View view;
	view.reset(sf::FloatRect(0, 0, target->getSize().x, target->getSize().y));
	view.setCenter(player.cameraPos);
	view.zoom(player.cameraZoom);

	target->setView(view);

	if (board != NULL)
	{
		board->draw(target);
	}

	// Draw empires
	for (size_t i = 0; i < empires.size(); i++)
	{
		empires[i].draw(target, board->spriteSheet, board->spriteSide);
	}
}

bool GameState::getAnyKeyDown(std::vector<Key>* keys)
{
	std::vector<Key> pKeys = *keys;
	for (size_t i = 0; i < pKeys.size(); i++)
	{
		if (sf::Keyboard::isKeyPressed(pKeys[i]))
		{
			return true;
		}
	}

	return false;
}

GameState::GameState(SettingsManager* sets)
{
	this->sets = sets;
}


GameState::~GameState()
{
	delete board;
}
