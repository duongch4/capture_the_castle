#include <time.h>
#include "tilemap.hpp"

bool Tilemap::init()
{
	int data[] = {
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  1, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  3, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23, 23, 23, 23, 23, 19, 19, 19, 19, 19, 19, 19, 24, 24, 24, 24, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23,  5, 23,  5, 23, 19, 19,  5, 19,  1, 11,  3, 24, 24, 24,  5, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  7, 11, 15, 23, 13, 11, 11, 11, 15, 19,  6, 19,  6, 24, 10, 11,  9, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23, 23, 23, 23, 23, 19, 19, 19, 19,  6, 19, 17, 24, 24, 24,  6, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  7, 11, 11, 12, 23, 23, 10, 11,  3, 19,  6, 19, 19, 24,  5, 24,  6, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23, 23, 23, 23, 23, 19, 19,  6, 19, 13, 11, 11, 11,  9, 24,  6, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 17, 23,  1, 12, 23, 10, 11, 11,  9, 19, 19, 19, 19, 24,  6, 24,  6, 24, 17, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 23, 23,  6, 23, 23, 23, 19, 19,  6, 19, 19, 19,  5, 24,  6, 24, 17, 24, 24, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  5, 23,  6, 23, 23, 23,  5, 19,  6, 19,  1, 11,  9, 24,  6, 24, 24, 24,  5, 19, 19, 19, 19, 19,
//		19, 19, 19, 19, 19,  5, 19,  6, 19, 10, 11,  9, 19, 19, 19, 19, 19,  6, 19,  7, 11, 11, 19,  5, 19, 19, 19, 19, 19,
//		19, 19, 19, 19, 19,  6, 19,  6, 19, 19, 19,  6, 19, 19, 19,  5, 19,  6, 19,  6, 19, 19, 19,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23, 13, 11, 11, 11, 15, 19, 17, 19, 17, 19,  6, 24, 17, 24,  5, 24,  6, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19,  6, 23, 23, 23, 23, 23, 19, 19, 19, 19, 19, 19,  6, 24, 24, 24,  6, 24,  6, 19, 19, 19, 19, 19,
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
			new_tile.set_idx(std::pair<int, int>{j, i});
		}
	}

	//--------------------------------------------------------------------------
	// Initialize the render
	static const GLfloat screen_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer & Vertex Array creation
	glGenVertexArrays(1, &mesh.vao);
	glGenBuffers(1, &mesh.vbo);
	glBindVertexArray(mesh.vao);
	// Copy the vertex data into the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer_data), screen_vertex_buffer_data, GL_STATIC_DRAW);
	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// Unbind the vertex array to prevent accidental change
	glBindVertexArray(0);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("tilemap.vs.glsl"), shader_path("tilemap.fs.glsl")))
		return false;

	return true;
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

	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Tilemap::draw(const mat3& projection)
{
	glEnable(GL_DEPTH_TEST);

	// Setting shaders
	glUseProgram(effect.program);

	// Set screen_texture sampling to texture unit 0
	GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
	glUniform1i(screen_text_uloc, 0);

	// Draw
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6); // two triangle covering the whole screen
}

void Tilemap::draw_all_tiles(const mat3& projection)
{
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.draw(projection);
		}
	}
}

Tile Tilemap::get_tile(float positionX, float positionY)
{	
	// Convert to the array index of the tile
	int gridX = (int)std::round((positionX - 23) / 48);
	int gridY = (int)std::round((positionY - 23) / 48);

	return m_tiles[gridY][gridX]; 
}

Tile Tilemap::get_tile(const std::pair<int,int>& tile_idx) const
{
	return m_tiles[tile_idx.first][tile_idx.second];
}

std::vector<Tile> Tilemap::get_adjacent_tiles(float positionX, float positionY)
{
	// Convert to the array index of the tile
	int gridX = (int)std::round((positionX - 23) / 48);
	int gridY = (int)std::round((positionY - 23) / 48);

	// Get the adjacent 9 tiles and return it
	std::vector<Tile> adjacentTiles;
	for (int j = -1; j < 2; j++) {
		for (int i = -1; i < 2; i++) {
			int row = gridY + j;
			int col = gridX + i;
			adjacentTiles.emplace_back(m_tiles[row][col]);
		}
	}
	return adjacentTiles;
}

std::vector<Tile> Tilemap::get_adjacent_tiles(const Tile& tile)
{
	const std::pair<int, int> tile_idx = tile.get_idx();

	// Get the adjacent 9 tiles and return it
	std::vector<Tile> adjacentTiles;
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			int row = tile_idx.first + j;
			int col = tile_idx.second + i;
			adjacentTiles.emplace_back(m_tiles[row][col]);
		}
	}
	return adjacentTiles;
}

std::vector<Tile> Tilemap::get_adjacent_tiles_nesw(const Tile& tile)
{
	const std::pair<int, int> tile_idx = tile.get_idx();

	// Get the adjacent 4 tiles and return it
	std::vector<Tile> adjacentTiles;
	adjacentTiles.emplace_back(m_tiles[tile_idx.first - 1][tile_idx.second + 0]); // north
	adjacentTiles.emplace_back(m_tiles[tile_idx.first + 0][tile_idx.second + 1]); // east
	adjacentTiles.emplace_back(m_tiles[tile_idx.first + 1][tile_idx.second + 0]); // south
	adjacentTiles.emplace_back(m_tiles[tile_idx.first + 0][tile_idx.second - 1]); // west
	return adjacentTiles;
}

vec2 Tilemap::get_random_free_tile_position(MazeRegion mazeRegion)
{
	// Intialize a random seed for rand() 
	srand((unsigned int)(time(NULL)));
	bool resultNotFound = true;

	switch (mazeRegion)
	{
	case MazeRegion::PLAYER1:
		while (resultNotFound)
		{
			int randomX = rand() % 5 + 6; // [6..10]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall())
			{
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::PLAYER2:
		while (resultNotFound)
		{
			int randomX = rand() % 5 + 18; // [18..22]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall())
			{
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::BANDIT:
		while (resultNotFound)
		{
			int randomX = rand() % 7 + 11; // [11..17]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall())
			{
				return m_tiles[randomY][randomX].get_position();
			}
		}
	default:
		return { 0,0 };
	}
}


MazeRegion Tilemap::get_region(float positionX, float positionY)
{
	// Convert to the array index of the tile
	int gridX = (int)std::round((positionX - 23) / 48);
	int gridY = (int)std::round((positionY - 23) / 48);

	if (gridX < 11) {
		return MazeRegion::PLAYER1;
	}
	else if (gridX < 18) {
		return MazeRegion::BANDIT;
	}
	else {
		return MazeRegion::PLAYER2;
	}
}

MazeRegion Tilemap::get_region(const Tile& tile)
{
	int tile_col_idx = tile.get_idx().second;

	if (tile_col_idx < 11)
	{
		return MazeRegion::PLAYER1;
	}
	else if (tile_col_idx < 18)
	{
		return MazeRegion::BANDIT;
	}
	else
	{
		return MazeRegion::PLAYER2;
	}
}

bool Tilemap::spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY)
{
	Tile tile;
	if (tile.init(sprite_id, num_horizontal, num_vertical, width, gap_width))
	{
		m_tiles[gridY][gridX] = tile;
		return true;
	}
	fprintf(stderr, "Failed to spawn tile");
	return false;
}

std::pair<int,int> Tilemap::get_height_width()
{
	return { tilemap_height, tilemap_width }; // [row, col] style
}
