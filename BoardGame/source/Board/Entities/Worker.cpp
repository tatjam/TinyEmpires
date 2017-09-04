#include "Worker.h"



void Worker::tick(float dt)
{
	inAnim = 0;

	if (reached && !isOnPath() && inOrder)
	{
		if (current.type == BUILD)
		{
			// Do order
			Tile at = getOwner()->game->board->getTile((size_t)current.target.x, (size_t)current.target.y);
			if (at.onTop != NULL)
			{
				if (at.onTop->getOwner() == getOwner())
				{
					// Build such building
					if (!at.onTop->isBuilt())
					{
						inAnim = 1;

						at.onTop->build(getDestroyPower() * dt);
						animTimer += dt;
						if (animTimer >= 1.0f)
						{
							animTimer = 0.0f;
						}
					}
				}
			}
		}
		else if (current.type == ATTACK)
		{
			Tile at = getOwner()->game->board->getTile((size_t)current.target.x, (size_t)current.target.y);
			if (at.onTop != NULL)
			{
				if (at.onTop->getOwner() != getOwner())
				{
					inAnim = 2;

					at.onTop->damage(getBuildPower() * dt);
					animTimer += dt;
					if (animTimer >= 1.0f)
					{
						animTimer = 0.0f;
					}
				}
			}
		}
		else if (current.type == MINE)
		{
			// Check what we are mining
			Tile target = getOwner()->game->board->getTile(current.target.x, current.target.y);
			if (target.resourceHealth > 0.0f)
			{
				if (target.metal > 0 || target.tech > 0)
				{
					inAnim = 4;
				}
				else
				{
					inAnim = 3;
				}

				target.resourceHealth -= getMiningPower() * dt;

				if (target.resourceHealth <= 0.0f)
				{
					// Mark it as hauling resource
					target.resourceHealth = -5000.0f;

					// Will render as broken resources
					getOwner()->game->board->setTile(current.target.x, current.target.y, target, true);

					// Assign haul order on it
					giveOrder(HAUL, current.target, current.target);
				}
				else
				{
					getOwner()->game->board->setTile(current.target.x, current.target.y, target);
				}

				

				animTimer += dt;
				if (animTimer >= 1.0f)
				{
					animTimer = 0.0f;
				}
			}
		}
	}
	else
	{
		if (!isOnPath() && inOrder)
		{
			reached = true;
		}
		else
		{
			reached = false;
		}
	}

	if (hasOrder())
	{
		Order order = getActiveOrder();
		
		current = order;
		reached = false;
		inOrder = true;

		if(order.type == MOVE || order.type == MINE)
		{
			moveTo(order.moveTarget, getOwner()->game->sets->gSets.threadedPathfinding);
		}
		else if(order.type == BUILD || order.type == ATTACK)
		{
			// Will move us to adjacent tile
			moveTo(order.moveTarget, getOwner()->game->sets->gSets.threadedPathfinding);
		}
		else if (order.type == HAUL)
		{
			// TODO
		}

		finishOrder();
	}
}

void Worker::postDraw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{
	Entity::postDraw(target, spriteSheet, tileSide);


	if (isOnPath())
	{
		sf::Sprite indicator = sf::Sprite();
		indicator.setTexture(*spriteSheet);
		indicator.setTextureRect(sf::IntRect(31 * tileSide, 0 * tileSide, tileSide, tileSide));

		indicator.setPosition((getPosition().x + getOffset().x) * tileSide, (getPosition().y + getOffset().y) * tileSide);

		target->draw(indicator);
	}

	// Draw build anim
	if (inAnim != 0)
	{
		sf::Sprite anim = sf::Sprite();
		anim.setTexture(*spriteSheet);
		if (inAnim == 1)
		{
			anim.setTextureRect(sf::IntRect(31 * tileSide, 31 * tileSide, tileSide, tileSide));
		}
		else if(inAnim == 2)
		{
			anim.setTextureRect(sf::IntRect(30 * tileSide, 31 * tileSide, tileSide, tileSide));
		}
		else if (inAnim == 3)
		{
			anim.setTextureRect(sf::IntRect(29 * tileSide, 31 * tileSide, tileSide, tileSide));
		}
		else
		{
			anim.setTextureRect(sf::IntRect(28 * tileSide, 31 * tileSide, tileSide, tileSide));
		}

		sf::Vector2f startPos = { (float)getPosition().x * tileSide, (float)getPosition().y * tileSide };
		sf::Vector2f targetPos = { current.target.x * (float)tileSide, current.target.y * (float)tileSide };
		sf::Vector2f realPos; 
		// lerp between values
		realPos.x = (startPos.x * (1.0f - animTimer)) + (targetPos.x * animTimer);
		realPos.y = (startPos.y * (1.0f - animTimer)) + (targetPos.y * animTimer);

		anim.setPosition(realPos.x, realPos.y);
		anim.setColor(sf::Color(255, 255, 255, (1.0f - animTimer) * 255));

		target->draw(anim);
	}
}

Worker::~Worker()
{
}
