#include "GameState.h"



void GameState::update(float dt, sf::RenderWindow* win)
{
	maptimer += dt;

	// Update empires
	for (size_t i = 0; i < empires.size(); i++)
	{
		empires[i].update(dt);
	}

	if (maptimer > 0.5f)
	{
		renderMinimap();
		maptimer = 0.0f;
	}

	userUpdate(dt, win);
}

void GameState::cameraUpdate(float dt, sf::RenderWindow* win)
{
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
	if (mPos.x > 0 && mPos.x < (int)win->getSize().x && mPos.y > 0 && mPos.y < (int)win->getSize().y)
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

void GameState::renderMinimap()
{
	minimap.create(board->width, board->height);

	for (size_t x = 0; x < board->width; x++)
	{
		for (size_t y = 0; y < board->height; y++)
		{
			Tile t = board->getTile(x, y);
			size_t i = y * board->width + x;

			// TODO: Use player empire (may not be 0)
			uint8_t viewd = empires[0].viewFinal[i];

			sf::Color base;
			
			if (t.onTop != NULL)
			{
				base = sf::Color(240, 128, 42);
			}
			else if (t.entityOnTop != NULL)
			{
				base = sf::Color(80, 80, 255);
			}
			else if (t.wall != NO_WALL)
			{
				base = sf::Color(55, 70, 145);
			}
			else
			{
				if (t.floor == GRASS) { base = sf::Color(70, 139, 75); }
				else if (t.floor == DIRT) { base = sf::Color(115, 114, 59); }
				else if (t.floor == SAND) { base = sf::Color(213, 211, 139); }
				else if (t.floor == ROCK) { base = sf::Color(158, 158, 158); }
				else if (t.floor == SHALLOW_WATER) { base = sf::Color(78, 215, 231); }
				else if (t.floor == DEEP_WATER) { base = sf::Color(44, 149, 175); }
				else { base = sf::Color(255, 0, 255); }
			}

			base.a = 255;

			if (viewd == 0)
			{
				base = sf::Color(0, 0, 0);
			}
			else if (viewd == 1)
			{
				base = sf::Color(base.r / 2, base.g / 2, base.b / 2);
			}
			else
			{
				base = base;
			}

			minimap.setPixel(x, y, base);
		}
	}

	minimapT.loadFromImage(minimap);
}

void GameState::userUpdate(float dt, sf::RenderWindow* win)
{

	cameraUpdate(dt, win);


}

void GameState::start(sf::Texture* spriteSheet, size_t spriteWidth)
{
	board = new Board(3, 3, spriteSheet, spriteWidth);
	board->generate(GenSettings(54321, -1.0f));
}

void GameState::rerender()
{
	board->render();
}

void GameState::draw(sf::RenderTarget* target)
{
	sf::View view;
	view.reset(sf::FloatRect(0, 0, (float)target->getSize().x, (float)target->getSize().y));
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
		empires[i].drawView(target);
	}

}

void GameState::drawUI(sf::RenderTarget* target)
{
	// Use GUI view
	sf::View active = target->getView();
	sf::View gui = sf::View();
	gui.reset(sf::FloatRect(0.0f, 0.0f, target->getSize().x, target->getSize().y));

	target->setView(gui);

	// Draw boxes
	sf::RectangleShape box = sf::RectangleShape();
	box.setFillColor(sf::Color(154, 129, 100));
	box.setOutlineColor(sf::Color(126, 88, 88));
	box.setOutlineThickness(-5.0f);

	// 15% used by bottom box
	box.setSize(sf::Vector2f((float)target->getSize().x, (float)target->getSize().y * 0.15f));
	box.setPosition(0.0f, target->getSize().y - box.getSize().y);
	
	target->draw(box);

	// Minimap box, double of bottom box and square
	box.setSize(sf::Vector2f(box.getSize().y * 1.5f, box.getSize().y * 1.5f));
	box.setPosition(target->getSize().x - box.getSize().x, target->getSize().y - box.getSize().y);

	target->draw(box);

	// Draw minimap
	sf::Sprite minimapSr;
	minimapSr.setTexture(minimapT);
	
	float minimapX = (float)board->width;
	float minimapY = (float)board->height;

	// We want minimap to be the same pixels as minimap box
	float wantedX = target->getSize().y * 0.15f * 1.5f - 5.0f;
	float wantedY = target->getSize().y * 0.15f * 1.5f - 5.0f;

	float scaleX = wantedX / minimapX;
	float scaleY = wantedY / minimapY;

	minimapSr.setScale(scaleX, scaleY);
	minimapSr.setPosition(target->getSize().x - wantedX, target->getSize().y - wantedY);

	target->draw(minimapSr);

	// Restore game view
	target->setView(active);
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

void GameState::addEmpire()
{
	Empire n = Empire(this);
	empires.push_back(n);
}

GameState::GameState(SettingsManager* sets)
{
	this->sets = sets;
}


GameState::~GameState()
{
	for (size_t i = 0; i < empires.size(); i++)
	{
		empires[i].end();
	}

	delete board;
}
