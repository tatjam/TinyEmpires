#include "Building.h"

sf::Vector2i Building::getPosition()
{
	return pos;
}

void Building::setPosition(sf::Vector2i nPos)
{
	pos = nPos;
}

void Building::build(float increase)
{
	buildProgress += increase;
}

void Building::select(Empire* selector)
{
	selected = true;
	this->selector = selector;
}

void Building::unselect(Empire* selector)
{
	selected = false;
	this->selector = selector;
}

void Building::setTexRect(sf::IntRect nRect)
{
	texRect = nRect;
}

void Building::setBuildingTexRect(sf::IntRect nRect)
{
	buildingTexRect = nRect;
}

void Building::setColorTexRect(sf::IntRect nRect)
{
	colorTexRect = nRect;
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

sf::IntRect Building::getBuildingTexRect()
{
	return buildingTexRect;
}

sf::IntRect Building::getBuildingFinalRect(size_t tileSide)
{
	sf::IntRect out = buildingTexRect;

	out.top *= tileSide; out.left *= tileSide; out.height *= tileSide; out.width *= tileSide;

	return out;
}

sf::IntRect Building::getColorTexRect()
{
	return colorTexRect;
}

sf::IntRect Building::getColorFinalRect(size_t tileSide)
{
	sf::IntRect out = colorTexRect;

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
	if (isBuilt() || buildProgress >= maxHealth * 0.2f)
	{

		int rRadius = viewRadius;

		if (buildProgress < maxHealth)
		{
			rRadius = 1;
		}

		// Using getSize() crashes due to thread synchronization
		// TODO: look into it

		for (int x = 0; x < texRect.width; x++)
		{
			for (int y = 0; y < texRect.height; y++)
			{
				for (int ox = (getPosition().x + x) - rRadius; ox < (getPosition().x + x) + rRadius; ox++)
				{
					for (int oy = (getPosition().y + y) - rRadius; oy < (getPosition().y + y) + rRadius; oy++)
					{
						float dist = sqrtf(powf((float)(ox - (getPosition().x + x)), 2.0f) + powf((float)(oy - (getPosition().y + y)), 2.0f));
						if (std::roundf(dist) < rRadius)
						{
							int i = oy * attached->width + ox;
							if (i >= 0 && ox < (int)owner->game->board->width && oy < (int)owner->game->board->height
								&& ox >= 0 && oy >= 0)
							{
								owner->view[i] = 2;
							}
						}
					}
				}
			}
		}
		
	}
}

void Building::baseUpdate(float dt, Empire* owner)
{
	if (this->owner != owner)
	{
		this->owner = owner;
	}

	fireTimer += dt;

	if (fireTimer >= 1.5f)
	{
		fireTimer = 0.0f;
	}

	if (fireTimer >= 0.0f && fireTimer <= 0.5f)
	{
		fireFrame = 0;
	}
	else if (fireTimer > 0.5f && fireTimer <= 1.0f)
	{
		fireFrame = 1;
	}
	else
	{
		fireFrame = 2;
	}
}

void Building::draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{
	if (buildProgress < maxHealth * 0.4f)
	{
		srand(id);

		// Draw a bunch of sprites
		sprite.setTexture(*spriteSheet);

		for (size_t x = 0; x < getSize().x; x++)
		{
			for (size_t y = 0 ; y < getSize().y; y++)
			{
				sprite.setPosition((x + getPosition().x) * tileSide, (y + getPosition().y) * tileSide);

				if (rand() % 1000 >= 500)
				{
					// Wood bar
					sprite.setTextureRect(sf::IntRect(14 * tileSide, 2 * tileSide, tileSide, tileSide));
				}
				else
				{
					if (rand() % 1000 >= 350)
					{
						// Barrel
						sprite.setTextureRect(sf::IntRect(15 * tileSide, 2 * tileSide, tileSide, tileSide));
					}
					else
					{
						// Rock
						sprite.setTextureRect(sf::IntRect(13 * tileSide, 2 * tileSide, tileSide, tileSide));
					}
				}

				target->draw(sprite);
			}
		}
	}
	else
	{

		sf::IntRect rect;

		if (isBuilt())
		{
			rect = getFinalRect(tileSide);
		}
		else
		{
			rect = getBuildingFinalRect(tileSide);
		}

		sprite.setTexture(*spriteSheet);
		sprite.setTextureRect(rect);

		sprite.setPosition((float)(getPosition().x * tileSide), (float)(getPosition().y * tileSide));

		target->draw(sprite);

		// Draw color overlay

		sprite.setTextureRect(getColorFinalRect(tileSide));
		sprite.setColor(owner->color);
		target->draw(sprite);
		sprite.setColor(sf::Color::White);
	}

	if (isSelected())
	{
		if (selector == owner)
		{
			// Player selected his building
			sprite.setTextureRect(sf::IntRect(0, 9 * tileSide, tileSide, tileSide));
		}
		else
		{
			// Enemy building
			sprite.setTextureRect(sf::IntRect(0, 10 * tileSide, tileSide, tileSide));
		}

		// Top left
		sprite.setTextureRect(sf::IntRect(12 * tileSide, sprite.getTextureRect().top, tileSide, tileSide));
		sprite.setPosition(getPosition().x * tileSide, getPosition().y * tileSide);
		target->draw(sprite);

		// Top right
		sprite.setTextureRect(sf::IntRect(13 * tileSide, sprite.getTextureRect().top, tileSide, tileSide));
		sprite.setPosition((getPosition().x + getSize().x - 1) * tileSide, getPosition().y * tileSide);
		target->draw(sprite);

		// Bottom left
		sprite.setTextureRect(sf::IntRect(14 * tileSide, sprite.getTextureRect().top, tileSide, tileSide));
		sprite.setPosition(getPosition().x * tileSide, (getPosition().y + getSize().y - 1) * tileSide);
		target->draw(sprite);

		// Bottom right
		sprite.setTextureRect(sf::IntRect(15 * tileSide, sprite.getTextureRect().top, tileSide, tileSide));
		sprite.setPosition((getPosition().x + getSize().x - 1) * tileSide, (getPosition().y + getSize().y - 1) * tileSide);
		target->draw(sprite);
	}
}

void Building::drawDamage(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{
	sf::Sprite spr;
	spr.setTexture(*spriteSheet);
	spr.setTextureRect(sf::IntRect((13 + fireFrame) * tileSide, 1 * tileSide, tileSide, tileSide));

	
	bool doNow = false;

	for (size_t x = 0; x < getSize().x; x++)
	{
		for (size_t y = 0; y < getSize().y; y++)
		{

			spr.setPosition((x + getPosition().x) * tileSide, (y + getPosition().y) * tileSide);

			if (health >= maxHealth * 0.5f && health <= maxHealth * 0.8f)
			{
				// Fire on alternative tiles
				if (doNow)
				{
					target->draw(spr);
					doNow = false;
				}
				else
				{
					doNow = true;
				}
			}
			else if (health <= maxHealth * 0.5f)
			{
				// One fire for each tile
				target->draw(spr);
			}
		}
	}
	
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

	getOwner()->removeBuilding(this);
}

void Building::die()
{
	// Fill ground with dirt tiles
	for (int ox = getPosition().x; ox < getPosition().x + getSize().x; ox++)
	{
		for (int oy = getPosition().y; oy < getPosition().y + getSize().y; oy++)
		{
			if (ox >= 0 && oy >= 0 && ox < owner->game->board->width && oy < owner->game->board->height)
			{
				Tile t = owner->game->board->getTile(ox, oy);
				t.floor = DIRT;
				owner->game->board->setTile(ox, oy, t);
			}
		}
	}

	// Rerender
	owner->game->board->renderChunks(
		owner->game->board->chunksContainedBy((sf::Vector2u)getPosition(), (sf::Vector2u)getSize()));
}

void Building::setMaxHealth(float val, bool resetHealth)
{
	maxHealth = val;

	if (resetHealth)
	{
		health = maxHealth;
	}
}

void Building::damage(float damage)
{
	if (isBuilt())
	{
		health -= damage;

		if (health <= 0.0f)
		{
			die();
			end();
		}
	}
	else
	{
		buildProgress -= damage;
		if (buildProgress <= 0.0f)
		{
			die();
			end();
		}
	}
}

void Building::setHealth(float val)
{
	if (isBuilt())
	{
		health = val;

		if (health <= 0.0f)
		{
			die();
			end();
		}
	}
	else
	{
		buildProgress = val;
		if (buildProgress <= 0.0f)
		{
			die();
			end();
		}
	}
}

