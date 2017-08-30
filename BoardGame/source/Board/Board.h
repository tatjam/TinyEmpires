#pragma once

#include <iostream>
#include <vector>
#include "SFML/Graphics.hpp"
#include "../stb/stb_perlin.h"
#include "Building.h"
#include <algorithm>

class Building;
class Entity;

// Spritesheet must contain 16*16 tiles

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define PATH_MAX_IT 10000


enum FloorType
{
	GRASS,
	DIRT,
	SAND,
	ROCK,
	SHALLOW_WATER,
	DEEP_WATER,
	DEBUG_TILE
};

enum WallType
{
	NO_WALL,
	ROCK_WALL,

};

enum ResourceSize
{
	NONE,
	SMALL,
	BIG
};



///////////////////////////////////////////////
/// \brief Defines a single Tile
///////////////////////////////////////////////
struct Tile
{
	FloorType floor = GRASS;
	WallType wall = NO_WALL;

	// Resources: 
	//	Only one can exist on each tile
	ResourceSize wood = NONE;
	ResourceSize tech = NONE;
	ResourceSize metal = NONE;
	ResourceSize food = NONE;

	// Owner ID
	// 0 - Wilderness
	size_t owner = 0;

	// Variate resource image?
	bool variation = false;

	float resourceHealth = 0.0f;

	// Building on top of this tile
	Building* onTop = NULL;
	Entity* entityOnTop = NULL;

	Tile(){}
	Tile(FloorType floor, WallType wall)
	{
		this->floor = floor;
		this->wall = wall;
	}


};

struct GenSettings
{
	float seed;

	float scale;
	float z, z1, z2, z3, z4;

	float mountainMin;
	float grassMin;
	float shallowMin;

	float minRock;
	float minMud;
	float minSand;

	float offx, offy, offx1, offy1;

	size_t wood, metal, food, tech;
	size_t bmetal, bfood, btech;

	bool ocean;


	void applySeed()
	{
		//srand((int)seed);
		z *= seed; z1 += seed; z2 /= seed; z3 = seed + z3 * seed; z4 *= seed;
		/*offx += (rand() % 256) / 2.0f; offy += (rand() % 256) / 2.0f;
		offx1 -= (rand() % 256) / 4.0f; offy1 -= (rand() % 256) / 4.0f;*/
		/*offx = seed * seed; offy = seed;
		offx1 = sqrtf(seed); offy1 = seed + seed + offx1;*/
	}

	GenSettings(float seed)
	{
		this->seed = seed;

		scale = 0.1f; z = 1.0f; z1 = 7.2f; z2 = 14.6f; z3 = 3.0f; z4 = 65.3f;
		offx = 0.0f; offy = 0.0f; offx1 = 0.0f; offy1 = 0.0f;

		mountainMin = 0.22f;
		grassMin = 0.0f;
		shallowMin = -0.3f;

		minRock = 0.46f;
		minMud = 0.45f;
		minSand = 0.46f;

		wood = 200;
		tech = 980; btech = 990;
		metal = 930; bmetal = 950;
		food = 940; bfood = 960;

		ocean = true;

		applySeed();
	}

	GenSettings(float seed, float sea)
	{
		this->seed = seed;

		scale = 0.1f; z = 1.0f; z1 = 7.2f; z2 = 14.6f; z3 = 3.0f; z4 = 65.3f;
		offx = 0.0f; offy = 0.0f; offx1 = 0.0f; offy1 = 0.0f;

		mountainMin = 0.22f + (sea/2);
		grassMin = 0.0f + (sea*1.2f);
		shallowMin = -0.3f + sea;

		minRock = 0.46f;
		minMud = 0.45f;
		minSand = 0.46f;

		wood = 200;
		tech = 980; btech = 990;
		metal = 930; bmetal = 950;
		food = 925; bfood = 940;

		ocean = true;

		applySeed();
	}
};

typedef std::vector<Tile> Chunk;


// If a Path is empty it means there is no 
// way to reach destination
typedef std::vector<sf::Vector2u> Path;

// Negative values mean impassable
struct PathCosts
{
	int ground = 100;
	int tree = 400;
	int shallow = 500;
	int deep = 1000;
	int path = 50;
	int wall = -1;
};

///////////////////////////////////////////////
/// \brief Holds and defines a board
///
/// Defines the board the game plays on
///////////////////////////////////////////////
class Board
{
private:



	std::vector<sf::Texture> renderedChunks;
	sf::RenderTexture* target;
	std::vector<Chunk> chunks;

	std::vector<float> heightmap;

	void renderChunk(size_t i, sf::RenderTarget* target);

	sf::IntRect obtainRect(FloorType t, bool overWater);

	sf::IntRect obtainRect(WallType t, bool overLand);


	std::vector<float> generateHeightmap(GenSettings set);
	void writeHeightmap(std::vector<float> heightmap, GenSettings set);
	void specialize(GenSettings settings);
	void placeResources(GenSettings settings);

	void drawResource(Tile* t, size_t x, size_t y, sf::RenderTarget* target);

	Tile nullTile = Tile(DEBUG_TILE, NO_WALL);

public:

	size_t width, height, cwidth, cheight;
	size_t spriteSide;
	sf::Texture* spriteSheet;

	// Re-renders the whole tileset
	void render();

	// Just draws to screen, set rect to required chunks to render
	void draw(sf::RenderTarget* target);

	void initialize(size_t xChunks, size_t yChunks, sf::Texture* spriteSheet, size_t spriteWidth);

	void setTile(size_t x, size_t y, Tile t, bool forceRender = false);

	Tile getTile(size_t x, size_t y);

	void generate(GenSettings setings);

	std::vector<sf::Vector2u> getNeighbors(sf::Vector2u tile, bool diagonal = false);

	size_t findIndex(sf::Vector2u vec);

	Path findPath(sf::Vector2u a, sf::Vector2u b, PathCosts costs = PathCosts(), size_t maxIterations = PATH_MAX_IT);

	// Uses real distance
	float pathDistance(sf::Vector2u a, sf::Vector2u b, PathCosts costs);

	// Uses heuristic distance
	float pathDistanceHeuristic(sf::Vector2u a, sf::Vector2u b);

	Board(size_t xChunks, size_t yChunks, sf::Texture* spriteSheet, size_t spriteWidth);
	~Board();
};

