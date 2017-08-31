#include "DemoEntity.h"



void DemoEntity::pretick(float dt)
{
}

void DemoEntity::tick(float dt)
{
	if (getMovementStatus())
	{
		if (sf::Keyboard::isKeyPressed(ST_KEY::I)) { setMovementOffset({ 0, -1 }); }
		if (sf::Keyboard::isKeyPressed(ST_KEY::J)) { setMovementOffset({ -1, 0 }); }
		if (sf::Keyboard::isKeyPressed(ST_KEY::K)) { setMovementOffset({ 0, 1 }); }
		if (sf::Keyboard::isKeyPressed(ST_KEY::L)) { setMovementOffset({ 1, 0 }); }

	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && win != NULL && win->hasFocus())
	{

		if (!isOnPath())
		{
			// Path to clicked
			sf::Vector2i pos = sf::Mouse::getPosition(*win);
			sf::Vector2f world = win->mapPixelToCoords(pos);

			sf::Vector2i targetTile = sf::Vector2i(world.x / tileSide, world.y / tileSide);

			//startPath(getOwner()->game->board->findPath((sf::Vector2u)getPosition(), (sf::Vector2u)targetTile, getPathCosts()));
			moveTo(targetTile, true);
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		cancelPath();
	}
}

void DemoEntity::start()
{
}

void DemoEntity::end()
{
}

void DemoEntity::draw(sf::RenderTarget* target, sf::Texture * spriteSheet, size_t tileSide)
{
	Entity::draw(target, spriteSheet, tileSide);
	this->win = (sf::RenderWindow*)target;
	this->tileSide = tileSide;
}

DemoEntity::~DemoEntity()
{
}
