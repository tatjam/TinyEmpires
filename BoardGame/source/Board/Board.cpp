#define STB_PERLIN_IMPLEMENTATION
#include "Board.h"


void Board::drawResource(Tile* t, size_t x, size_t y, sf::RenderTarget* target)
{
	sf::Sprite spr;
	spr.setTexture(*spriteSheet);

	bool draw = false;

	if (t->wall == NO_WALL)
	{
		if (t->wood != NONE)
		{
			if (t->variation)
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 1, spriteSide * 2, spriteSide, spriteSide));
			}
			else
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 0, spriteSide * 2, spriteSide, spriteSide));
			}

			draw = true;
		}
		else if (t->metal != NONE)
		{
			if (t->metal == SMALL)
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 6, spriteSide * 2, spriteSide, spriteSide));
			}
			else
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 3, spriteSide * 2, spriteSide, spriteSide));
			}

			draw = true;
		}
		else if (t->tech != NONE)
		{
			if (t->tech == SMALL)
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 5, spriteSide * 2, spriteSide, spriteSide));
			}
			else
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 2, spriteSide * 2, spriteSide, spriteSide));
			}

			draw = true;
		}
		else if (t->food != NONE)
		{
			if (t->food == SMALL)
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 7, spriteSide * 2, spriteSide, spriteSide));
			}
			else
			{
				spr.setTextureRect(sf::IntRect(spriteSide * 4, spriteSide * 2, spriteSide, spriteSide));
			}

			draw = true;
		}

		if (draw)
		{
			spr.setPosition((float)(x * spriteSide), (float)(y * spriteSide));
			target->draw(spr);
		}
	}
}

void Board::drawOverload(Tile we, Tile them, size_t loc, int x, int y, sf::RenderTarget* target)
{
	if (we.floor == them.floor || 
		x < 0 || y < 0 || x > width || y > height ||
		we.wall != NO_WALL || them.wall != NO_WALL)
	{
		return;
	}

	// Dirt overloads nothing
	if (we.floor == DIRT)
	{
		return;
	}
	// Grass overloads dirt
	else if (we.floor == GRASS)
	{
		if (them.floor != DIRT)
		{
			return;
		}
	}
	// Sand overloads grass and dirt
	else if (we.floor == SAND)
	{
		if (them.floor != GRASS && them.floor != DIRT)
		{
			return;
		}
	}
	// Rock overloads everything
	else if(we.floor == ROCK)
	{
		// don't return; (:P)
	}
	else
	{
		return;
	}

	// Draw the thing

	sf::Sprite spr;
	spr.setTexture(*spriteSheet);

	if (we.floor == DIRT)
	{
		// Not possible uh :P
		spr.setTextureRect(sf::IntRect(0, spriteSide * 14, spriteSide, spriteSide));
	}
	else if (we.floor == GRASS)
	{
		spr.setTextureRect(sf::IntRect(0, spriteSide * 12, spriteSide, spriteSide));
	}
	else if (we.floor == SAND)
	{
		spr.setTextureRect(sf::IntRect(0, spriteSide * 14, spriteSide, spriteSide));
	}
	else
	{
		spr.setTextureRect(sf::IntRect(0, spriteSide * 15, spriteSide, spriteSide));
	}

	if (loc == 0)
	{
		spr.setTextureRect(sf::IntRect(12 * spriteSide, spr.getTextureRect().top, spriteSide, spriteSide));
	}
	else if (loc == 1)
	{
		spr.setTextureRect(sf::IntRect(13 * spriteSide, spr.getTextureRect().top, spriteSide, spriteSide));
	}
	else if (loc == 2)
	{
		spr.setTextureRect(sf::IntRect(14 * spriteSide, spr.getTextureRect().top, spriteSide, spriteSide));
	}
	else
	{
		spr.setTextureRect(sf::IntRect(15 * spriteSide, spr.getTextureRect().top, spriteSide, spriteSide));
	}

	spr.setPosition(x * spriteSide, y * spriteSide);

	target->draw(spr);

}

void Board::renderChunk(size_t i, sf::RenderTarget* target)
{
	// Grass goes over dirt 
	// Sand goes over grass and dirt
	// Rock goes over everything
	// Dirt goes over nothing

	sf::Sprite spr;
	spr.setTexture(*spriteSheet, true);

	for (size_t x = 0; x < CHUNK_WIDTH; x++)
	{
		for (size_t y = 0; y < CHUNK_HEIGHT; y++)
		{
			size_t chunkx = i % cwidth;
			size_t chunky = i / cwidth;

			size_t rx = x + (chunkx * CHUNK_WIDTH);
			size_t ry = y + (chunky * CHUNK_HEIGHT);

			spr.setPosition((float)(x * spriteSide), (float)(y * spriteSide));

			size_t j = y * CHUNK_WIDTH + x;
			Tile t = chunks[i][j];

			if (t.wall == NO_WALL)
			{
				// Floor
				bool overWater = false;

				if (getTile(rx, ry + 1).floor == SHALLOW_WATER || getTile(rx, ry + 1).floor == DEEP_WATER)
				{
					overWater = true;
				}

				spr.setTextureRect(obtainRect(t.floor, overWater));
				target->draw(spr);
			}
			else
			{
				// Wall
				bool overGround = false;

				if (getTile(rx, ry + 1).wall == NO_WALL)
				{
					overGround = true;
				}

				spr.setTextureRect(obtainRect(t.wall, overGround));
				target->draw(spr);
			}

			
		}
	}

	for (size_t x = 0; x < CHUNK_WIDTH; x++)
	{
		for (size_t y = 0; y < CHUNK_HEIGHT; y++)
		{
			size_t chunkx = i % cwidth;
			size_t chunky = i / cwidth;

			size_t rx = x + (chunkx * CHUNK_WIDTH);
			size_t ry = y + (chunky * CHUNK_HEIGHT);

			// Draw tile overloads
			for (int ox = -1; ox <= 1; ox++)
			{
				for (int oy = -1; oy <= 1; oy++)
				{
					if (ox == -1 && oy == 0)
					{
						// Left
						drawOverload(getTile(rx, ry), getTile(rx + ox, ry + oy), 1, (int)x + (int)ox, (int)y + (int)oy, target);
					}
					else if (ox == 1 && oy == 0)
					{
						// Right
						drawOverload(getTile(rx, ry), getTile(rx + ox, ry + oy), 3, (int)x + (int)ox, (int)y + (int)oy, target);
					}
					else if (ox == 0 && oy == 1)
					{
						// Down
						drawOverload(getTile(rx, ry), getTile(rx + ox, ry + oy), 0, (int)x + (int)ox, (int)y + (int)oy, target);
					}
					else if (ox == 0 && oy == -1)
					{
						// Top
						drawOverload(getTile(rx, ry), getTile(rx + ox, ry + oy), 2, (int)x + (int)ox, (int)y + (int)oy, target);
					}

				}
			}
		}
	}

	// Draw resources afterwards
	for (size_t x = 0; x < CHUNK_WIDTH; x++)
	{
		for (size_t y = 0; y < CHUNK_HEIGHT; y++)

		{
			size_t j = y * CHUNK_WIDTH + x;
			Tile t = chunks[i][j];

			drawResource(&t, x, y, target);
		}
	}
}

sf::IntRect Board::obtainRect(FloorType t, bool overWater)
{
	if(t == GRASS)
	{
		if(!overWater){ return sf::IntRect(0, 0, spriteSide, spriteSide); } 
		else { return sf::IntRect(6 * spriteSide, 0, spriteSide, spriteSide); }

	}
	else if (t == DIRT)
	{
		if (!overWater) { return sf::IntRect(spriteSide, 0, spriteSide, spriteSide); }
		else { return sf::IntRect(7 * spriteSide, 0, spriteSide, spriteSide); }
	}
	else if (t == SAND)
	{
		if (!overWater) { return sf::IntRect(spriteSide * 2, 0, spriteSide, spriteSide); }
		else { return sf::IntRect(8 * spriteSide, 0, spriteSide, spriteSide); }
	}
	else if (t == ROCK)
	{
		if (!overWater) { return sf::IntRect(spriteSide * 3, 0, spriteSide, spriteSide); }
		else { return sf::IntRect(9 * spriteSide, 0, spriteSide, spriteSide); }
	}
	else if (t == SHALLOW_WATER)
	{
		return sf::IntRect(spriteSide * 4, 0, spriteSide, spriteSide);
	}
	else if (t == DEEP_WATER)
	{
		return sf::IntRect(spriteSide * 10, 0, spriteSide, spriteSide);
	}
	else if (t == DEBUG_TILE)
	{
		return sf::IntRect(spriteSide * 15, 0, spriteSide, spriteSide);
	}
	else
	{
		return sf::IntRect(0, 0, spriteSide, spriteSide);
	}
}

sf::IntRect Board::obtainRect(WallType t, bool overGround)
{
	if (t == ROCK_WALL)
	{
		if (overGround)
		{
			return sf::IntRect(spriteSide, spriteSide, spriteSide, spriteSide);
		}
		else
		{
			return sf::IntRect(0, spriteSide, spriteSide, spriteSide);
		}

	}
	else
	{
		return sf::IntRect(0, spriteSide, spriteSide, spriteSide);
	}
}



void Board::render()
{
	std::cout << "Whole rerender started" << std::endl;


	// Render all the chunks
	for (size_t i = 0; i < cwidth * cheight; i++)
	{
		target->clear();
		
		renderChunk(i, target);

		target->display();

		sf::Texture nTex = target->getTexture();
		renderedChunks[i] = nTex;

	}

	
}

void Board::draw(sf::RenderTarget* target)
{
	sf::Sprite chunk;

	for (size_t x = 0; x < cwidth; x++)
	{
		for (size_t y = 0; y < cheight; y++)
		{
			size_t i = y * cwidth + x;

			chunk.setPosition(x * CHUNK_WIDTH * spriteSide, y * CHUNK_HEIGHT * spriteSide);
			chunk.setTexture(renderedChunks[i]);

			target->draw(chunk);
		}
	}
}

void Board::initialize(size_t x, size_t y, sf::Texture* img, size_t spriteWidth)
{
	spriteSheet = img;
	width = x * CHUNK_WIDTH;
	height = y * CHUNK_HEIGHT;

	cwidth = x;
	cheight = y;

	target = new sf::RenderTexture();
	target->create(CHUNK_WIDTH * spriteWidth, CHUNK_HEIGHT * spriteWidth);

	renderedChunks = std::vector<sf::Texture>();
	renderedChunks.resize(cwidth * cheight);

	chunks = std::vector<Chunk>();
	chunks.resize(cwidth * cheight);

	for (size_t i = 0; i < chunks.size(); i++)
	{
		chunks[i] = Chunk();
		chunks[i].resize((CHUNK_WIDTH * CHUNK_HEIGHT) + 1);
	}

	Tile def = Tile();

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			setTile(x, y, def);
		}
	}

	spriteSide = spriteWidth;
}

void Board::setTile(size_t x, size_t y, Tile t, bool reRender)
{
	if (x > width || y > height)
	{
		return;
	}
	else
	{

		size_t chunkX = x / CHUNK_WIDTH;
		size_t chunkY = y / CHUNK_WIDTH;
		size_t chunkI = chunkY * cwidth + chunkX;

		size_t offX = x - (chunkX * CHUNK_WIDTH);
		size_t offY = y - (chunkY * CHUNK_HEIGHT);
		size_t offI = offY * CHUNK_WIDTH + offX;

		Chunk* c = &chunks[chunkI];
		c->at(offI) = t;

		if (reRender)
		{
			target->clear();
			renderChunk(chunkI, target);
			target->display();

			renderedChunks[chunkI] = target->getTexture();
		}
	}
}

Tile Board::getTile(size_t x, size_t y)
{
	if (x > width || y > height)
	{
		return nullTile;
	}
	else
	{
		x %= width;
		y %= height;

		size_t chunkX = x / CHUNK_WIDTH;
		size_t chunkY = y / CHUNK_WIDTH;
		size_t chunkI = chunkY * cwidth + chunkX;

		size_t offX = x - (chunkX * CHUNK_WIDTH);
		size_t offY = y - (chunkY * CHUNK_HEIGHT);
		size_t offI = offY * CHUNK_WIDTH + offX;

		Chunk* c = &chunks[chunkI];
		return c->at(offI);
	}
}

void Board::generate(GenSettings settings)
{
	std::cout << "Generating (" << cwidth << " x " << cheight << ") chunks" << std::endl;

	std::cout << "000% - ";
	std::vector<float> heightMap = generateHeightmap(settings);
	std::cout << "025% - ";
	writeHeightmap(heightMap, settings);
	std::cout << "050% - ";
	specialize(settings);
	std::cout << "075% - ";
	placeResources(settings);
	std::cout << "100% - Completed" << std::endl;
}

std::vector<sf::Vector2u> Board::getNeighbors(sf::Vector2u tile, bool diagonal)
{
	std::vector<sf::Vector2u> out;

	if (tile.x < width && tile.y < height)
	{

		if (diagonal)
		{
			std::cout << "IMPLEMENT DIAGONAL" << std::endl;
		}
		else
		{
			sf::Vector2u up = { tile.x, tile.y - 1 };
			sf::Vector2u right = { tile.x + 1, tile.y };
			sf::Vector2u down = { tile.x, tile.y + 1 };
			sf::Vector2u left = { tile.x - 1, tile.y };

			if (up.x < width && up.y < height) { out.push_back(up); }
			if (right.x < width && right.y < height) { out.push_back(right); }
			if (down.x < width && down.y < height) { out.push_back(down); }
			if (left.x < width && left.y < height) { out.push_back(left); }
		}

	}

	return out;
}

size_t Board::findIndex(sf::Vector2u vec)
{
	return (vec.y * width + vec.x);
}

Path Board::findPath(sf::Vector2u start, sf::Vector2u end, PathCosts costs, size_t maxIterations)
{

	// Implemented the pseudocode from wikipedia

	Path out;



	if (start.x < width && start.y < height && end.x < width && end.y < height)
	{
		// First check for reachability of goal to stop very long searches
		// Fast check, check that neighbor tiles to start and end are walkable
		std::vector<sf::Vector2u> startNeighbors = getNeighbors(start);
		std::vector<sf::Vector2u> endNeighbors = getNeighbors(end);
		
		if (pathDistance(start, start, costs) == INFINITY || pathDistance(end, end, costs) == INFINITY)
		{
			return out;
		}

		bool any = false;

		for (size_t i = 0; i < startNeighbors.size(); i++)
		{
			float dist = pathDistance(start, startNeighbors[i], costs);
			if (dist != INFINITY)
			{
				any = true;
			}
		}

		if (any == false)
		{
			std::cout << "FAILURE" << std::endl;
			return out;
		}

		any = false;

		for (size_t i = 0; i < endNeighbors.size(); i++)
		{
			float dist = pathDistance(start, endNeighbors[i], costs);
			if (dist != INFINITY)
			{
				any = true;
			}
		}

		if (any == false)
		{
			std::cout << "FAILURE" << std::endl;
			return out;
		}

		std::vector<sf::Vector2u> closed;

		std::vector<sf::Vector2u> frontier;
		frontier.push_back(start);

		// The map of costs
		std::vector<float> gScore;
		gScore.resize(width * height, INFINITY);
		gScore[findIndex(start)] = 0.0f;

		std::vector<float> fScore;
		fScore.resize(width * height, INFINITY);
		fScore[findIndex(start)] = pathDistanceHeuristic(start, end);

		std::vector<sf::Vector2u> cameFrom;
		cameFrom.resize(width * height);


		size_t iterations = 0;

		// While frontier is not empty
		while (frontier.size() > 0)
		{
			iterations++;

			// Find lowest value in frontier
			float lowest = INFINITY;
			sf::Vector2u current;
			for (size_t i = 0; i < frontier.size(); i++)
			{
				float n = fScore[findIndex(frontier[i])];
				if (n <= lowest)
				{
					current = frontier[i];
					lowest = n;
				}
			}

			if (current == end || iterations >= maxIterations)
			{
				// Construct the Path

				out.push_back(current);

				bool done = false;

				while (!done)
				{
					current = cameFrom[findIndex(current)];

					// End early to not include start point
					if (current == start)
					{
						done = true;
						break;
					}

					out.push_back(current);
				}

				// Reverse the path (go from start to end)
				Path buff = Path();
				for (int i = out.size() - 1; i >= 0; i--)
				{
					buff.push_back(out[i]);
				}

				if (iterations >= maxIterations)
				{
					std::cout << "ITERATION OVERLOAD" << std::endl;
				}
				else
				{
					std::cout << "SUCCESS" << std::endl;
				}

				return buff;
			}

			// Remove current from frontier
			frontier.erase(std::remove(frontier.begin(), frontier.end(), current), frontier.end());
			// Add current to closed
			closed.push_back(current);

			std::vector<sf::Vector2u> neighbors = getNeighbors(current);

			for (size_t i = 0; i < neighbors.size(); i++)
			{
				sf::Vector2u neighbor = neighbors[i];

				// Check if we are already contained in closed
				if (std::find(closed.begin(), closed.end(), neighbor) != closed.end())
				{
					// We are contained, do nothing
					continue;
				}
				else
				{
					// Add to frontier
					frontier.push_back(neighbor);
				}

				// Distance from start to neighbor
				float nG = gScore[findIndex(current)];
				float pD = pathDistance(current, neighbor, costs);

				float tentativeGScore = nG + pD;
				if (tentativeGScore > gScore[findIndex(neighbor)])
				{
					// Sub-optimal path
					continue;
				}

				// Record path until now
				cameFrom[findIndex(neighbor)] = current;
				gScore[findIndex(neighbor)] = tentativeGScore;
				fScore[findIndex(neighbor)] = tentativeGScore + pathDistanceHeuristic(neighbor, end);
			}

		}
	}

	std::cout << "FAILURE" << std::endl;
	return out;
}

std::vector<float> Board::generateHeightmap(GenSettings settings)
{
	std::cout << "Generating heightmap" << std::endl;

	std::vector<float> heightMap;
	heightMap.resize(width * height);

	size_t written = 0;
	size_t ccount = 0;

	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			sf::Vector2f center = { roundf((float)width) / 2, roundf((float)height) / 2 };
			float distFromCenter = sqrtf(powf((float)x - center.x, 2.0f) + powf((float)y - center.y, 2.0f));

			float ox = (float)x * settings.scale;
			float oy = (float)y * settings.scale;
			ox += settings.offx;
			oy += settings.offy;

			float val = stb_perlin_noise3(ox, oy, settings.z, 256, 256, 256);
			float val2 = stb_perlin_fbm_noise3(ox + settings.offx1, oy + settings.offy1, settings.z1, 2.0f, 0.5f, 6, 256, 256, 256);
			val2 -= 0.2f;
			val = (val + val + val2) / 3;;

			// Ocean map
			float ocean = stb_perlin_noise3((float)x * 0.05f, (float)y * 0.05f, settings.z4, 256, 256, 256);
			ocean *= 3.0f;

			if (ocean > 0.1f)
			{
				if (ocean > 0.4f)
				{
					ocean = 0.4f;
				}

				if (settings.ocean)
				{
					val -= ocean;
				}
			}

			heightMap[y * width + x] = val - ((distFromCenter / (width / 1.2f)));

			/*if (distFromCenter > width / 2.5f || true)
			{
				heightMap[y * width + x] = val - ((distFromCenter / (width / 1.2f)));
			}
			else 
			{
				heightMap[y * width + x] = val;
			}*/

			//heightMap[y * width + x] = val - (distFromCenter*distFromCenter) / 60000.0f;
			//heightMap[y * width + x] = val * (-(distFromCenter*distFromCenter) / 60000.0f);

			written++;
		}
	}

	return heightMap;
}

void Board::writeHeightmap(std::vector<float> heightmap, GenSettings settings)
{
	std::cout << "Writting heightmap" << std::endl;


	size_t written = 0;
	size_t ccount = 0;

	this->heightmap = heightmap;

	Tile mountain = Tile(GRASS, ROCK_WALL);
	Tile grass = Tile(GRASS, NO_WALL);
	Tile shallow = Tile(SHALLOW_WATER, NO_WALL);
	Tile deep = Tile(DEEP_WATER, NO_WALL);

	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			size_t i = y * width + x;
			float val = heightmap[i];

			if (val > settings.mountainMin)
			{
				setTile(x, y, mountain);
			}
			else if (val > settings.grassMin)
			{
				setTile(x, y, grass);
			}
			else if (val > settings.shallowMin)
			{
				setTile(x, y, shallow);
			}
			else
			{
				setTile(x, y, deep);
			}

		}
	}
}

void Board::specialize(GenSettings settings)
{
	std::cout << "Detailing surface" << std::endl;

	size_t written = 0;
	size_t ccount = 0;

	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			float rock = stb_perlin_fbm_noise3(x * settings.scale, y * settings.scale, settings.z2, 
				2.0f, 0.5f, 6, 256, 256, 256);

			float mud = stb_perlin_noise3(x * settings.scale, y * settings.scale, settings.z3, 
				256, 256, 256);

			Tile actual = getTile(x, y);

			if (mud > settings.minMud)
			{
				if (actual.floor != SHALLOW_WATER && actual.floor != DEEP_WATER)
				{
					actual.floor = DIRT;
				}
			}

			if (rock > settings.minRock)
			{
				if (actual.floor != SHALLOW_WATER && actual.floor != DEEP_WATER)
				{
					actual.floor = ROCK;
				}
			}

			setTile(x, y, actual);

			/*if (sand > settings.minSand)
			{
				if (actual->floor != SHALLOW_WATER || actual->floor != DEEP_WATER)
				{
					actual->floor = SAND;
				}
			}*/

		}
	}

	written = 0;
	ccount = 0;

	// Apply sand beaches
	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			// Beach if it touches any water tile
			Tile we = getTile(x, y);
			if (we.floor != SHALLOW_WATER && we.floor != DEEP_WATER)
			{
				bool touches = false;

				Tile top = getTile(x, y - 1);
				Tile right = getTile(x + 1, y);
				Tile down = getTile(x, y + 1);
				Tile left = getTile(x - 1, y);
				Tile topright = getTile(x + 1, y - 1);
				Tile topleft = getTile(x - 1, y - 1);
				Tile botright = getTile(x + 1, y + 1);
				Tile botleft = getTile(x - 1, y + 1);

				if (top.floor == SHALLOW_WATER || top.floor == DEEP_WATER) { touches = true; }
				if (right.floor == SHALLOW_WATER || right.floor == DEEP_WATER) { touches = true; }
				if (down.floor == SHALLOW_WATER || down.floor == DEEP_WATER) { touches = true; }
				if (left.floor == SHALLOW_WATER || left.floor == DEEP_WATER) { touches = true; }
				if (topright.floor == SHALLOW_WATER || topright.floor == DEEP_WATER) { touches = true; }
				if (topleft.floor == SHALLOW_WATER || topleft.floor == DEEP_WATER) { touches = true; }
				if (botright.floor == SHALLOW_WATER || botright.floor == DEEP_WATER) { touches = true; }
				if (botleft.floor == SHALLOW_WATER || botleft.floor == DEEP_WATER) { touches = true; }

				if (touches)
				{
					we.floor = SAND;
				}

				setTile(x, y, we);

			}
		}
	}

}

void Board::placeResources(GenSettings settings)
{
	std::cout << "Placing resources" << std::endl;

	srand((int)settings.seed*25);
	
	float wood = 1.0f - ((1000 - (float)settings.wood) / 1000.0f);

	size_t written = 0;
	size_t ccount = 0;

	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{

			bool placed = false;

			Tile we = getTile(x, y);

			float forest = stb_perlin_turbulence_noise3(x * settings.scale * 10, y * settings.scale * 10, settings.z4,
				2.0f, 0.03f, 6, 256, 256, 256);

			if (we.floor == GRASS && !placed && we.wall == NO_WALL)
			{
				if (forest > wood)
				{
					we.wood = SMALL;
					placed = true;
				}
				else if (forest > wood + 0.15f)
				{
					we.wood = BIG;
					placed = true;
				}
			}

			if (we.floor == ROCK && !placed)
			{
				if (rand() % 1000 > (int)settings.tech)
				{
					// Place tech
					we.tech = SMALL;
					placed = true;
				}
				else if (rand() % 1000 > (int)settings.btech)
				{
					we.tech = BIG;
					placed = true;
				}
			}

			if (we.floor == ROCK && !placed)
			{
				if (rand() % 1000 > (int)settings.metal)
				{
					// Place metal
					we.metal = SMALL;
					placed = true;
				}
				else if (rand() % 1000 > (int)settings.bmetal)
				{
					we.metal = BIG;
					placed = true;
				}
			}

			if ((we.floor == GRASS || we.floor == DIRT) && !placed && we.wall == NO_WALL)
			{
				if (rand() % 1000 > (int)settings.food)
				{
					// Place food
					we.food = SMALL;
					placed = true;
				}
				else if (rand() % 1000 > (int)settings.bfood)
				{
					we.food = BIG;
					placed = true;
				}
			}


			if (rand() % 1000 > 800)
			{
				we.variation = true;
			}

			setTile(x, y, we);



		}
	}
}



float Board::pathDistance(sf::Vector2u a, sf::Vector2u b, PathCosts costs)
{
	// Find distance
	if (a.x > width || a.y > height || b.x > width || b.y > height)
	{
		return INFINITY;
	}
	else
	{
		// Basic distance
		/*float dist = (a.x - b.x) + (a.y - b.y);
		dist = std::abs(dist);*/

		// We could add diagnoals
		float dist = 1.0f;

		// Add costs
		Tile target = getTile(b.x, b.y);
		if (target.wall != NO_WALL || target.onTop != NULL)
		{
			if (costs.wall < 0){dist = INFINITY;}
			else{dist *= costs.wall;}
		}
		else if(target.floor == GRASS || target.floor == DIRT || target.floor == SAND)
		{
			if (costs.ground < 0) { dist = INFINITY; }
			else { dist *= costs.ground; }
		}
		else if (target.floor == ROCK)
		{
			if (costs.path < 0) { dist = INFINITY; }
			else { dist *= costs.path; }
		}
		else if (target.floor == SHALLOW_WATER)
		{
			if (costs.shallow < 0) { dist = INFINITY; }
			else { dist *= costs.shallow; }
		}
		else if (target.floor == DEEP_WATER)
		{
			if (costs.deep < 0) { dist = INFINITY; }
			else { dist *= costs.deep; }
		}
		else
		{
			dist = INFINITY;
		}

		return dist;
	}


}

float Board::pathDistanceHeuristic(sf::Vector2u a, sf::Vector2u b)
{
	float dist = ((float)a.x - (float)b.x) + ((float)a.y - (float)b.y);
	return std::abs(dist);
}

Board::Board(size_t x, size_t y, sf::Texture* img, size_t spriteWidth)
{
	initialize(x, y, img, spriteWidth);
}


Board::~Board()
{
}
