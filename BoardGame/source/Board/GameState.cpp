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
	otimer += dt;

	// Dispatch orders
	if (otimer >= 0.25f)
	{
		if (ordersBeingGiven.size() > 0)
		{
			std::pair<Entity*, Order> order = ordersBeingGiven[ordersBeingGiven.size() - 1];
			ordersBeingGiven.pop_back();

			order.first->giveOrder(order.second.type, order.second.target);
			otimer = 0.0f;
		}


	}

	cameraUpdate(dt, win);

	// Mouse clicks

	// Square selector
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (inSelection)
		{
			// Draw rectangle
			newPos = (sf::Vector2f)sf::Mouse::getPosition(*win);
			newPos = win->mapPixelToCoords((sf::Vector2i)newPos);

		}
		else
		{
			if (!getAnyKeyDown(&sets->cSets.multiSelectKeys))
			{

				entitiesOnly = false;

				// Remove previous selections
				for (size_t i = 0; i < selected.size(); i++)
				{
					selected[i]->unselect(&empires[0]);
				}

				// Remove previous selections
				for (size_t i = 0; i < selectedBuildings.size(); i++)
				{
					selectedBuildings[i]->unselect(&empires[0]);
				}


				selected.clear();
				selectedBuildings.clear();

			}

			clickPos = (sf::Vector2f)sf::Mouse::getPosition(*win);
			clickPos = win->mapPixelToCoords((sf::Vector2i)clickPos);
			newPos = clickPos;

			inSelection = true;
		}
	}
	else
	{
		if (inSelection)
		{
			// Finish selection

			int lowestX = std::min(clickPos.x, newPos.x);
			int highestX = std::max(clickPos.x, newPos.x);
			int lowestY = std::min(clickPos.y, newPos.y);
			int highestY = std::max(clickPos.y, newPos.y);

			if (lowestX < 0) { lowestX = 0; }
			if (highestX < 0) { highestX = 0; }
			if (lowestY < 0) { lowestY = 0; }
			if (highestY < 0) { highestY = 0; }

			lowestX /= board->spriteSide;
			lowestY /= board->spriteSide;
			highestX /= board->spriteSide;
			highestY /= board->spriteSide;

			if (highestX == lowestX)
			{
				highestX = lowestX + 1;
			}

			if (highestY == lowestY)
			{
				highestY = lowestY + 1;
			}


			// Selection will prioritize entities


			for (size_t x = (size_t)lowestX; x < (size_t)highestX; x++)
			{
				for (size_t y = (size_t)lowestY; y < (size_t)highestY; y++)
				{
					Tile t = board->getTile(x, y);
					if (t.entityOnTop != NULL)
					{
						// TODO: Player empire
						t.entityOnTop->select(&empires[0]);

						selected.push_back(t.entityOnTop);

						// Clear buildings
						for (size_t i = 0; i < selectedBuildings.size(); i++)
						{
							selectedBuildings[i]->unselect(&empires[0]);
						}
						selectedBuildings.clear();

						entitiesOnly = true;
					}

					if (t.onTop != NULL && !entitiesOnly)
					{
						t.onTop->select(&empires[0]);
						/*if (!(std::find(selectedBuildings.begin(), selectedBuildings.end(), x) != selectedBuildings.end()))
						{
							selectedBuildings.push_back(t.onTop);
						}*/

						selectedBuildings.push_back(t.onTop);
						
					}
				}
			}

			inSelection = false;
		}
	}

	if (inAnim)
	{
		animProgress += 3 * dt;
		if (animProgress >= 1.0f)
		{
			inAnim = false;
			animProgress = 0.0f;
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if (!held)
		{
			if (!selected.empty())
			{
				sf::Vector2i nAnimTarget = sf::Mouse::getPosition(*win);
				sf::Vector2f nAnimTargetf = win->mapPixelToCoords(nAnimTarget);
				nAnimTarget = { (int)(nAnimTargetf.x / board->spriteSide), (int)(nAnimTargetf.y / board->spriteSide) };

				for (size_t i = 0; i < selected.size(); i++)
				{
					Order order; order.type = MOVE; order.target = nAnimTarget;
					ordersBeingGiven.push_back(std::pair<Entity*, Order>(selected[i], order));
					//selected[i]->giveOrder(MOVE, nAnimTarget);
				}

				if (nAnimTarget != animTarget)
				{
					inAnim = true;
					animProgress = 0.0f;

					animTarget = nAnimTarget;
				}
			}

			held = true;
		}
	}
	else 
	{
		held = false;
	}

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

	if (inAnim)
	{
		float dist = 1.0f - (1.0f * (animProgress * 0.7f));
		sf::Sprite arrow = sf::Sprite();
		arrow.setTexture(*board->spriteSheet);
		
		// Top left arrow
		arrow.setTextureRect(sf::IntRect(14 * board->spriteSide, 8 * board->spriteSide, board->spriteSide, board->spriteSide));
		sf::Vector2f pos = { (animTarget.x - dist) * board->spriteSide, (animTarget.y - dist) * board->spriteSide };
		arrow.setPosition(pos);
		target->draw(arrow);

		// Top right
		arrow.setTextureRect(sf::IntRect(13 * board->spriteSide, 8 * board->spriteSide, board->spriteSide, board->spriteSide));
		pos = { (animTarget.x + dist) * board->spriteSide, (animTarget.y - dist) * board->spriteSide };
		arrow.setPosition(pos);
		target->draw(arrow);

		// Bot left
		arrow.setTextureRect(sf::IntRect(12 * board->spriteSide, 8 * board->spriteSide, board->spriteSide, board->spriteSide));
		pos = { (animTarget.x - dist) * board->spriteSide, (animTarget.y + dist) * board->spriteSide };
		arrow.setPosition(pos);
		target->draw(arrow);

		// Bot right
		arrow.setTextureRect(sf::IntRect(15 * board->spriteSide, 8 * board->spriteSide, board->spriteSide, board->spriteSide));
		pos = { (animTarget.x + dist) * board->spriteSide, (animTarget.y + dist) * board->spriteSide };
		arrow.setPosition(pos);
		target->draw(arrow);

	}

}

void GameState::drawUI(sf::RenderTarget* target)
{
	sf::Vector2f topleft = target->mapPixelToCoords({ 0, 0 });
	sf::Vector2f botright = target->mapPixelToCoords((sf::Vector2i)target->getSize());

	if (inSelection)
	{
		sf::RectangleShape selectionRect;
		selectionRect.setFillColor(sf::Color::Transparent);
		selectionRect.setOutlineColor(sf::Color::White);
		selectionRect.setOutlineThickness(-1.0f);
		selectionRect.setPosition(clickPos);
		selectionRect.setSize(sf::Vector2f(newPos.x - clickPos.x, newPos.y - clickPos.y));

		target->draw(selectionRect);

	}

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

	// Draw view rectangle


	sf::Vector2i min = { (int)std::roundf(topleft.x / board->spriteSide), (int)std::roundf(topleft.y / board->spriteSide) };
	sf::Vector2i max = { (int)std::roundf(botright.x / board->spriteSide), (int)std::roundf(botright.y / board->spriteSide) };

	min.x = std::max((float)min.x, 0.0f);
	min.y = std::max((float)min.y, 0.0f);
	max.x = std::max((float)max.x, 0.0f);
	max.x = std::max((float)max.x, 0.0f);

	sf::RectangleShape viewRect = sf::RectangleShape();
	viewRect.setFillColor(sf::Color::Transparent);
	viewRect.setOutlineColor(sf::Color::White);
	viewRect.setOutlineThickness(-1.0f);

	float xPos = (target->getSize().x - wantedX) + (min.x * scaleX);
	float yPos = (target->getSize().y - wantedY) + (min.y * scaleY);
	float xSize = (max.x - min.x) * scaleX;
	float ySize = (max.y - min.y) * scaleY;

	viewRect.setPosition(xPos, yPos);
	viewRect.setSize(sf::Vector2f(xSize, ySize));
	
	target->draw(viewRect);

	// Draw cursor
	/*
	sf::Sprite sprite = sf::Sprite();
	sprite.setTexture(*board->spriteSheet);
	sprite.setTextureRect(sf::IntRect(31 * board->spriteSide, 31 * board->spriteSide, board->spriteSide, board->spriteSide));

	sprite.setPosition((sf::Vector2f)sf::Mouse::getPosition(*(sf::RenderWindow*)target));
	target->draw(sprite);
	*/

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
