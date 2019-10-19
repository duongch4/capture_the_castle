#include <time.h>
#include "tilemap.hpp"

bool Tilemap::init()
{
	int data[] = {
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  1, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  3, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19,  5, 19,  5, 19, 19, 19,  5, 19,  1, 11,  3, 19, 19, 19,  5, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  7, 11, 15, 19, 13, 11, 11, 11, 15, 19,  6, 19,  6, 19, 10, 11,  9, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19, 19, 19, 19, 19,  6, 19, 17, 19, 19, 19,  6, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  7, 11, 11, 12, 19, 19, 10, 11,  3, 19,  6, 19, 19, 19,  5, 19,  6, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19, 19, 19,  6, 19, 13, 11, 11, 11,  9, 19,  6, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 17, 19,  1, 12, 19, 10, 11, 11,  9, 19, 19, 19, 19, 19,  6, 19,  6, 19, 17, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19,  6, 19, 19, 19,  5, 19,  6, 19, 17, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  5, 19,  6, 19, 19, 19,  5, 19,  6, 19,  1, 11,  9, 19,  6, 19, 19, 19,  5, 19, 19, 19, 19, 19,
		//19, 19, 19, 19, 19,  5, 19,  6, 19, 10, 11,  9, 19, 19, 19, 19, 19,  6, 19,  7, 11, 11, 19,  5, 19, 19, 19, 19, 19,
		//19, 19, 19, 19, 19,  6, 19,  6, 19, 19, 19,  6, 19, 19, 19,  5, 19,  6, 19,  6, 19, 19, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19, 13, 11, 11, 11, 15, 19, 17, 19, 17, 19,  6, 19, 17, 19,  5, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  6, 19, 19, 19,  6, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 13, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 14, 11, 11, 11, 14, 11, 15, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	};

	tilemap_height = 17;
	tilemap_width = 29;

	for (int j = 0; j < tilemap_height; j++) {
		m_tiles.emplace_back(std::vector<Tile>(tilemap_width));
		for (int i = 0; i < tilemap_width; i++) {
			int iter = j * tilemap_width + i;
			// First parameter is the id of the tile, second parameter is the number of tile horizontally in the sprite sheet
			// Third parameter is the number of tile vertically in the sprite sheet.
			if (!spawn_tile(data[iter], 6, 4, 68, 20, i, j)) {
				return false;
			}

			Tile& new_tile = m_tiles[j][i];

			// Setting the tile initial position
			new_tile.set_position({ i * 48.f + 23.f, j * 48.f + 23.f });
		}
	}
	return false;
}


void Tilemap::destroy()
{
    for (auto &vector : m_tiles) {
        for (auto &tile : vector) {
            tile.destroy();
        }
    }

    for (auto &vector : m_tiles) {
        vector.clear();
    }

    m_tiles.clear();
}

void Tilemap::draw(const mat3& projection)
{
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.draw(projection);
		}
	}
}

Tile Tilemap::get_tile(int positionX, int positionY)
{	
	// Convert to the array index of the tile
	int gridX = (positionX - 23) / 48;
	int gridY = (positionY - 23) / 48;
	return m_tiles[gridY][gridX]; 
}

std::vector<Tile> Tilemap::get_adjacent_tiles(int positionX, int positionY)
{ 
	// Convert to the array index of the tile
	int gridX = (positionX - 23) / 48;
	int gridY = (positionY - 23) / 48;

	// Get the adjacent 9 tiles and return it
	std::vector<Tile> adjacentTiles;
	for (int j = -1; j < 2; j++) {
		for (int i = -1; i < 2; i++) {
			adjacentTiles.emplace_back(m_tiles[gridY + j][gridX + i]);
		}
	}
	return adjacentTiles;
}

vec2 Tilemap::get_random_free_tile_position(MazeRegion mazeRegion) 
{
	// Intialize a random seed for rand() 
	srand(time(NULL));
	bool resultNotFound = true;

	switch (mazeRegion) {
	case MazeRegion::PLAYER1:
		while (resultNotFound) {
			int randomX = rand() % 5 + 6; // [6..11]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::PLAYER2:
		while (resultNotFound) {
			int randomX = rand() % 6 + 17; // [17..22]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::BANDIT:
		while (resultNotFound) {
			int randomX = rand() % 5 + 12; // [12..16]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
				return m_tiles[randomY][randomX].get_position();
			}
		}
	}
}

MazeRegion Tilemap::get_region(int positionX, int positionY) 
{
	// Convert to the array index of the tile
	int gridX = (positionX - 23) / 48;
	int gridY = (positionY - 23) / 48;

	if (gridX < 12) {
		return MazeRegion::PLAYER1;
	}
	else if (gridX < 17) {
		return MazeRegion::BANDIT;
	}
	else {
		return MazeRegion::PLAYER2;
	}
}

bool Tilemap::spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY)
{
	Tile tile;
	if (tile.init(sprite_id, num_horizontal, num_vertical, width, gap_width))
	{
		m_tiles[gridY].emplace(m_tiles[gridY].begin() + gridX, tile);
		return true;
	}
	fprintf(stderr, "Failed to spawn tile");
	return false;
}

