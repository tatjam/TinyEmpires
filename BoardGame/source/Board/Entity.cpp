#include "Entity.h"



sf::Vector2i Entity::getPosition()
{
	return position;
}

bool Entity::setPosition(sf::Vector2i nPos, bool set)
{
	Tile actual = owner->game->board->getTile(getPosition().x, getPosition().y);
	Tile newPos = owner->game->board->getTile(nPos.x, nPos.y);

	if (newPos.entityOnTop != NULL && newPos.entityOnTop != this)
	{
		return false;
	}

	actual.entityOnTop = NULL;
	newPos.entityOnTop = this;



	if (set)
	{
		owner->game->board->setTile(getPosition().x, getPosition().y, actual);
		owner->game->board->setTile(nPos.x, nPos.y, newPos);


		position = nPos;
	}

	return true;
}

void Entity::setTexPos(sf::Vector2u pos)
{
	texPos = pos;
}

sf::Vector2u Entity::getTexPos()
{
	return texPos;
}

void Entity::setColorTexPos(sf::Vector2u pos)
{
	colorTexPos = pos;
}

sf::Vector2u Entity::getColorTexPos()
{
	return colorTexPos;
}

sf::IntRect Entity::getTextureRect(size_t tileSide, bool color)
{
	if (color)
	{
		return sf::IntRect(colorTexPos.x * tileSide, colorTexPos.y * tileSide, tileSide, tileSide);
	}
	else
	{
		return sf::IntRect(texPos.x * tileSide, texPos.y * tileSide, tileSide, tileSide);
	}
}

size_t Entity::getViewRadius()
{
	return viewRadius;
}

void Entity::setViewRadius(size_t r)
{
	viewRadius = r;
}

void Entity::fillView()
{
	for (int ox = getPosition().x - (int)viewRadius; ox < getPosition().x + (int)viewRadius; ox++)
	{
		for (int oy = getPosition().y - (int)viewRadius; oy < getPosition().y + (int)viewRadius; oy++)
		{
			float dist = sqrtf(powf(ox - getPosition().x, 2.0f) + powf(oy - getPosition().y, 2.0f));
			if (std::roundf(dist) < viewRadius)
			{
				int i = oy * owner->game->board->width + ox;
				if (i >= 0 && i < owner->game->board->width * owner->game->board->height
					&& ox < owner->game->board->width && oy < owner->game->board->height)
				{
					owner->view[i] = 2;
				}
			}
		}
	}
}

void Entity::draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{
	sprite.setTexture(*spriteSheet);
	sprite.setTextureRect(getTextureRect(tileSide));

	sprite.setPosition((float)getPosition().x * (float)tileSide + offset.x * (float)tileSide,
		(float)getPosition().y * (float)tileSide + offset.y * (float)tileSide);

	target->draw(sprite);

	sprite.setTextureRect(getTextureRect(tileSide, true));
	sprite.setColor(owner->color);

	target->draw(sprite);

	sprite.setColor(sf::Color::White);

	if (selected)
	{
		// Draw selected overlay

		if (selector == owner)
		{
			// Blue selector
			sprite.setTextureRect(sf::IntRect(13 * tileSide, 11 * tileSide, tileSide, tileSide));
		}
		else
		{
			// Red selector (we are enemies of player)
			sprite.setTextureRect(sf::IntRect(12 * tileSide, 11 * tileSide, tileSide, tileSide));
		}

		target->draw(sprite);
	}
}

void Entity::select(Empire* selector)
{
	selected = true;
	this->selector = selector;
}

void Entity::unselect(Empire* selector)
{
	selected = false;
	this->selector = NULL;
}

void Entity::damage(int damage)
{
	health -= damage;

	if (health <= 0)
	{
		end();
	}
}

void Entity::giveOrder(OrderType order, sf::Vector2i target, sf::Vector2i moveTarget)
{
	activeOrder.type = order;
	activeOrder.target = target;
	activeOrder.moveTarget = moveTarget;
}

void Entity::finishOrder()
{
	activeOrder.type = NO_ORDER;
}

Order Entity::getActiveOrder()
{
	return activeOrder;
}

bool Entity::hasOrder()
{
	return activeOrder.type != NO_ORDER;
}

void Entity::updateMovement(float dt)
{
	if (inPath && prevPos == getPosition())
	{
		timeInStaticPath += dt;

		if (timeInStaticPath > 3.0f)
		{
			moveTo((sf::Vector2i)wantedTarget);
			std::cout << "Unstuck" << std::endl;
			timeInStaticPath = 0.0f;
		}
	}
	else
	{
		timeInStaticPath = 0.0f;
	}


	if (inPath && getPath()->at(getPath()->size() - 1) == (sf::Vector2u)getPosition())
	{
		activePath.clear();
		inPath = false;
		pathptr = 0;
		//moveTo((sf::Vector2i)wantedTarget);
		std::cout << "Unstuck" << std::endl;
	}

	if (inMovement)
	{

		if (setPosition(movTarget, false) == false)
		{
			// Oh, we are fucked, we are moving into 
			// an unit! SOMEWHAT >:(
			interpTo({ 0, 0 }, movProgress);
			movProgress -= rspeed * dt;

			return;
		}

		sf::Vector2i target = getPosition() - movTarget;
		movProgress += rspeed * dt;

		interpTo(target, movProgress);



		if (movProgress >= 1.0f)
		{

			if (setPosition(movTarget) == false)
			{
				// lol wut?
			}
			else
			{
				// Reset
				inMovement = false;
				movProgress = 0.0f;
				interpTo({ 0, 0 }, 0.0f);
			}
		}
	}

	prevPos = getPosition();
}

void Entity::updatePathing(float dt)
{
	if (threadWorking)
	{
		if (threadWorkerFinished)
		{
			worker->join();
			delete worker;

			threadWorking = false;

			startPath(activePath);

		}
	}
	else
	{
		if (inPath)
		{
			if (getMovementStatus())
			{
				if (activePath.size() > 0 && pathptr < activePath.size())
				{
					sf::Vector2u next = activePath[pathptr];

					Tile nextT = owner->game->board->getTile(next.x, next.y);

					if (nextT.entityOnTop != NULL && nextT.entityOnTop != this)
					{
						// Wait until timeout
						timeWaited += dt;

						if (timeWaited >= 0.25f)
						{
							timeWaited = 0.0f;
							activePath = getOwner()->game->board->findPath((sf::Vector2u)getPosition(), wantedTarget, getPathCosts(), 10000, this);
							if (activePath.empty())
							{
								inPath = false;
								pathptr = 0;
								setMovementTarget(getPosition());
								return;
							}
							pathptr = 0;
							return;
						}
						else
						{
							setMovementTarget(getPosition());
							return;
						}
					}
					else if (setMovementTarget((sf::Vector2i)next) == 0.0f)
					{
						if (next == wantedTarget)
						{
							// We did enough
							setMovementTarget(getPosition());
							inPath = false;
							pathptr = 0;
							return;
						}
						else
						{
							// Find new path:
							moveTo((sf::Vector2i)wantedTarget, owner->game->sets->gSets.threadedPathfinding);
							//activePath = getOwner()->game->board->findPath((sf::Vector2u)getPosition(), wantedTarget, getPathCosts(), 10000, this);
							if (activePath.size() <= 0)
							{
								inPath = false;
								return;
							}
							pathptr = 0;
							return;
						}
					}

					pathptr++;

					if (pathptr >= activePath.size())
					{
						inPath = false;
						pathptr = 0;
					}
				}
				else
				{
					inPath = false;
					pathptr = 0;
				}
			}
		}
	}
}

float Entity::setMovementTarget(sf::Vector2i tile)
{
	if (tile != getPosition())
	{
		Tile targetTile = owner->game->board->getTile(tile.x, tile.y);

		if ((targetTile.wall != NONE || targetTile.onTop != NULL) && !flyer || targetTile.entityOnTop != NULL)
		{
			rspeed = 0.0f;
		}
		else if (targetTile.floor == GRASS || targetTile.floor == DIRT
			|| targetTile.floor == SAND)
		{
			if (targetTile.wood != NONE)
			{
				rspeed = speed * treeMult;
			}
			else
			{
				rspeed = speed * groundMult;
			}
		}
		else if (targetTile.floor == ROCK)
		{
			rspeed = speed * pathMult;
		}
		else if (targetTile.floor == SHALLOW_WATER)
		{
			rspeed = speed * shallowMult;
		}
		else if (targetTile.floor == DEEP_WATER)
		{
			rspeed = speed * deepMult;
		}
		else
		{
			rspeed = 0.0f;
		}

		if (rspeed > 0.0f)
		{
			inMovement = true;
			movTarget = tile;
		}
		else
		{
			inMovement = false;
			movTarget = getPosition();
		}

		return rspeed;
	}


}

float Entity::setMovementOffset(sf::Vector2i offset)
{
	return setMovementTarget(sf::Vector2i(getPosition() + offset));
}

bool Entity::getMovementStatus()
{
	return !inMovement;
}

void Entity::moveTo(sf::Vector2i target, bool threaded)
{
	wantedTarget = (sf::Vector2u)target;

	if (threaded)
	{
		inPath = false;
		pathptr = 0;

		threadWorkerFinished = false;
		threadWorking = true;

		worker = new std::thread(computePath, getOwner(),
			&activePath, (sf::Vector2u)getPosition(), (sf::Vector2u)target, *generatedCosts, &threadWorkerFinished, this);
	}
	else
	{
		startPath(owner->game->board->findPath((sf::Vector2u)getPosition(), (sf::Vector2u)target, *generatedCosts, 10000, this));
	}
}

void Entity::startPath(std::vector<sf::Vector2u> p)
{
	if (p.size() != 0)
	{
		cancelPath();
		inPath = true;
		activePath = p;
		pathptr = 0;
		wantedTarget = p[p.size() - 1];
	}
}

void Entity::cancelPath()
{
	if (inPath)
	{
		inPath = false;
		pathptr = 0;
	}
}

bool Entity::isOnPath()
{
	return inPath;
}

std::vector<sf::Vector2u>* Entity::getPath()
{
	return &activePath;
}

float Entity::getSpeed()
{
	return speed;
}

void Entity::setSpeed(float speed)
{
	this->speed = speed;
}

sf::Vector2f Entity::interpTo(sf::Vector2i target, float t)
{
	if (t < 0.0f)
	{
		return sf::Vector2f(0, 0);
	}

	float x = 0;
	float y = 0;

	float ex = target.x;
	float ey = target.y;

	float ix = ((1 - t) * x) + (t * ex);
	float iy = ((1 - t) * y) + (t * ey);

	offset = sf::Vector2f(-ix, -iy);

	return offset;
}

void Entity::setOffset(sf::Vector2f of)
{
	offset = of;
}

sf::Vector2f Entity::getOffset()
{
	return offset;
}

int Entity::getHealth()
{
	return health;
}

int Entity::getMaxHealth()
{
	return maxHealth;
}

void Entity::setHealth(int h)
{
	health = h;
}

void Entity::setMaxHealth(int h, bool resetHealth)
{
	maxHealth = h;
	if (resetHealth)
	{
		health = h;
	}
}

PathCosts Entity::getPathCosts()
{
	if (generatedCosts == NULL)
	{
		generatedCosts = new PathCosts();
	}

	return *generatedCosts;
}

void Entity::generatePathCosts()
{
	// We use 100 as our precision

	if (generatedCosts == NULL)
	{
		generatedCosts = new PathCosts();
	}

	if (flyer)
	{
		generatedCosts->wall = 100;
	}
	else
	{
		generatedCosts->wall = -1;
	}

	if (groundMult == 0) { generatedCosts->ground = -1; }
	else {
		generatedCosts->ground = (1.0f / (float)groundMult) * 100;
	}

	if (pathMult == 0) { generatedCosts->path = -1; }
	else {
		generatedCosts->path = (1.0f / (float)pathMult) * 100;
	}

	if (shallowMult == 0) { generatedCosts->shallow = -1; }
	else {
		generatedCosts->shallow = (1.0f / (float)shallowMult) * 100;
	}

	if (deepMult == 0) { generatedCosts->deep = -1; }
	else {
		generatedCosts->deep = (1.0f / (float)deepMult) * 100;
	}

	if (treeMult == 0) { generatedCosts->tree = -1; }
	else {
		generatedCosts->tree = (1.0f / (float)treeMult) * 100;
	}

}

void Entity::setOwner(Empire* owner)
{
	this->owner = owner;
}

Empire* Entity::getOwner()
{
	return owner;
}

Entity::Entity(Empire* owner)
{
	this->owner = owner;
	threadWorking = false;
	inPath = false;
}


Entity::~Entity()
{
}

void computePath(Empire* owner, std::vector<sf::Vector2u>* out,
	sf::Vector2u from, sf::Vector2u to, PathCosts costs, bool* finished, Entity* thisEntity)
{
	std::vector<sf::Vector2u> path;
	path = owner->game->board->findPath(from, to, costs, 10000, thisEntity);

	*out = path;

	*finished = true;
}
