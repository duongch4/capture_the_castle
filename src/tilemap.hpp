#pragma once

#include "common.hpp"
#include "tile.hpp"
#include <vector>

class Tilemap 
{
public: 
	bool init();

	void destroy();

	// Draw the entire screen texture with the tiles
	void draw(const mat3& projection);
	// Draw each individual tile
	void draw_all_tiles(const mat3& projection);

	// Return the tile located at postionX, positionY
	Tile get_tile(float positionX, float positionY);

	// Return all 9 tiles around the position in a vector
	std::vector<Tile> get_adjacent_tiles(float positionX, float positionY);

	// Get a random free tile in the maze region and return their position 
	vec2 get_random_free_tile_position(MazeRegion mazeRegion);
	
	// Return the region based on the position 

	MazeRegion get_region(float positionX, float positionY);

	bool spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY);

	// Get tilemap width and height
	std::pair<int, int> get_height_width();

private:
	// Contain vao, vbo, shader to render the screen texture 
	Mesh mesh{};
	Effect effect{};

	// Amount of tile in the tile map horizontally 
	int tilemap_width;
	// Amount of tile in the tile map vertically 
	int tilemap_height;

	// All the tiles in a 2D vector
	std::vector<std::vector<Tile>> m_tiles;
};