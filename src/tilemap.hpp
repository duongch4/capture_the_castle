#pragma once

#include "common.hpp"
#include "tile.hpp"
#include <vector>

class Tilemap 
{
public: 
	bool init();

	void destroy();

	void draw(const mat3& projection);

	// Return the tile located at postionX, positionY
	Tile get_tile(int positionX, int positionY);

	// Return all 9 tiles around the position in a vector
	std::vector<Tile> get_adjacent_tiles(int positionX, int positionY);

	// Get a random free tile in the maze region and return their position 
	vec2 get_random_free_tile_position(MazeRegion mazeRegion);
	
	// Return the region based on the position 
	MazeRegion get_region(int positionX, int positionY);

	bool spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY);

private:
	// Amount of tile in the tile map horizontally 
	int tilemap_width;
	// Amount of tile in the tile map vertically 
	int tilemap_height;
	// All the tiles in a 2D vector
	std::vector<std::vector<Tile>> m_tiles;
};