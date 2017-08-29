#include "Building.h"

sf::Vector2i Building::getPosition()
{
	return pos;
}

void Building::setPosition(sf::Vector2i nPos)
{
	pos = nPos;
}

void Building::setTexRect(sf::IntRect nRect)
{
	texRect = nRect;
}

sf::IntRect Building::getTexRect()
{
	return texRect;
}

sf::IntRect Building::getFinalRect(size_t tileSide)
{
	sf::IntRect out = texRect;

	out.top *= tileSide; out.left *= tileSide; out.height *= tileSide; out.width *= tileSide;

	return out;
}

size_t Building::getViewRadius()
{
	return viewRadius;
}

void Building::setViewRadius(size_t i)
{
	viewRadius = i;
}

void Building::fillView(Empire* owner)
{
	for (int ox = getPosition().x - (int)viewRadius; ox < getPosition().x + (int)viewRadius; ox++)
	{
		for (int oy = getPosition().y - (int)viewRadius; oy < getPosition().y + (int)viewRadius; oy++)
		{
			float dist = sqrtf(powf((float)(ox - getPosition().x), 2.0f) + powf((float)(oy - getPosition().y), 2.0f));
			if (std::roundf(dist) < viewRadius)
			{
				int i = oy * attached->width + ox;
				if (i >= 0 && ox < (int)owner->game->board->width && oy < (int)owner->game->board->height)
				{
					owner->view[i] = 2;
				}
			}
		}
	}
}

void Building::draw(sf::RenderTarget * target, sf::Texture * spriteSheet, size_t tileSide)
{
	sf::IntRect rect = getFinalRect(tileSide);
	sprite.setTexture(*spriteSheet);
	sprite.setTextureRect(rect);

	sprite.setPosition((float)(getPosition().x * tileSide), (float)(getPosition().y * tileSide));

	target->draw(sprite);
}

void Building::start()
{

	for (size_t x = 0; x < (size_t)getTexRect().width; x++)
	{
		for (size_t y = 0; y < (size_t)getTexRect().height; y++)
		{
			size_t rx = getPosition().x + x;
			size_t ry = getPosition().y + y;

			Tile t = attached->getTile(rx, ry);
			t.onTop = this;
			attached->setTile(rx, ry, t);
		}
	}
}

void Building::end()
{
	for (size_t x = 0; x < (size_t)getTexRect().width; x++)
	{
		for (size_t y = 0; y < (size_t)getTexRect().height; y++)
		{
			size_t rx = getPosition().x + x;
			size_t ry = getPosition().y + y;

			Tile t = attached->getTile(rx, ry);
			t.onTop = NULL;
			attached->setTile(rx, ry, t);
		}
	}
}
