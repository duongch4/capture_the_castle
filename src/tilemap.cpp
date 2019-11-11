#include <time.h>
#include <cmath>
#include <iostream>
#include <string>
#include <random>
#include "tilemap.hpp"


bool Tilemap::init()
{
    std::vector<std::vector<int>> mazeData = this->load_map();

	tilemap_height = 17;
	tilemap_width = 29;

	// Create all the tiles based on the data we got from the .tmx file
	for (int j = 0; j < tilemap_height; j++) {
		m_tiles.emplace_back(std::vector<Tile>(tilemap_width));
		for (int i = 0; i < tilemap_width; i++) {
			if (!spawn_tile(mazeData[j][i], 6, 4, 68, 20, i, j)) {
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
	int gridX = ((int)std::ceil(positionX) - 23) / 48;
	int gridY = ((int)std::ceil(positionY) - 23) / 48;
	return m_tiles[gridY][gridX]; 
}

std::vector<Tile> Tilemap::get_adjacent_tiles(float positionX, float positionY)
{
	// Convert to the array index of the tile
	int gridX = ((int)std::ceil(positionX) - 23) / 48;
	int gridY = ((int)std::ceil(positionY) - 23) / 48;

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

std::vector<Tile> Tilemap::get_adjacent_tiles_wesn(float positionX, float positionY)
{
	// Convert to the array index of the tile
	int gridX = ((int)std::ceil(positionX) - 23) / 48;
	int gridY = ((int)std::ceil(positionY) - 23) / 48;

	// Get the adjacent 4 tiles and return it
	std::vector<Tile> adjacentTiles;
	adjacentTiles.emplace_back(m_tiles[gridY - 1][gridX + 0]); // north
	adjacentTiles.emplace_back(m_tiles[gridY + 0][gridX - 1]); // west
	adjacentTiles.emplace_back(m_tiles[gridY + 0][gridX + 1]); // east
	adjacentTiles.emplace_back(m_tiles[gridY + 1][gridX + 0]); // south
	return adjacentTiles;
}

vec2 Tilemap::get_random_free_tile_position(MazeRegion mazeRegion) 
{
	// Intialize a random seed for rand() 
	srand((unsigned int)(time(NULL)));
	bool resultNotFound = true;

	switch (mazeRegion) {
	case MazeRegion::PLAYER1:
		while (resultNotFound) {
			int randomX = rand() % 5 + 6; // [6..10]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::PLAYER2:
		while (resultNotFound) {
			int randomX = rand() % 5 + 18; // [18..22]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
				return m_tiles[randomY][randomX].get_position();
			}
		}
	case MazeRegion::BANDIT:
		while (resultNotFound) {
			int randomX = rand() % 7 + 11; // [11..17]
			int randomY = rand() % 11 + 4; // [4..14]
			if (!m_tiles[randomY][randomX].is_wall()) {
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
	int gridX = ((int)std::ceil(positionX) - 23) / 48;
	int gridY = ((int)std::ceil(positionY) - 23) / 48;

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

std::vector<std::vector<int>> Tilemap::load_map()
{
    std::vector<std::vector<int>> result;
    std::vector<std::string> stringArray;

//    //********************************************************************
//    // For gcc version 9.0 and above (it fixed a bug where file system cant be used by Window)
//    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78870
//    // This version is more robust and allow us to use any file name with .tmx format
//    // Comment out the other version below this, then uncomment this version and #include <filesystem> at the top of this file
//
//    // Get all the maze's file name and store them in a vector
//    std::vector<std::string> fileArray;
//    std::string mazePath = data_path "/maze";
//    std::string ext = ".tmx";
//    for (const auto& file : std::filesystem::directory_iterator(mazePath)) {
//        if (file.path().extension() == ext) {
//            fileArray.emplace_back((file.path().filename().string()));
//        }
//    }
//
//    // Randomly choose one of the maze to load
//    srand((unsigned int)(time(NULL)));
//    int randomMazeIndex = rand() % (fileArray.size()); // [0..fileArray.size() - 1]
//    std::string mazeName = fileArray[randomMazeIndex];
//    //********************************************************************

    //********************************************************************
    // For gcc version below 9.0
    // This version don't use <filesystem> and the file name is hardcoded

    // Hardcoded file name for the maze located in ./data/maze
    std::vector<std::string> fileArray = {"HelloWorld", "BanditCircus", "TwoByTwo"};
    std::string ext = ".tmx";

    // Randomly choose one of the maze to load
    std::mt19937 rng;
    rng.seed(time(NULL));
    std::uniform_int_distribution<int> distribution(0,fileArray.size() - 1);
    int randomMazeIndex = distribution(rng);
    std::string mazeName = fileArray[randomMazeIndex] + ext;

    //********************************************************************


    try {
        // Load the maze file
        std::string fileToLoad = maze_path() + mazeName;
        std::ifstream inputStream(fileToLoad);

        if (!inputStream) {
            std::cout << "Error opening file: " << fileToLoad << std::endl;
            throw "Error opening file: " + fileToLoad;
        }

        // Loop through the file line by line and retrieve the maze data
        bool foundData = false;
        for (std::string line; std::getline(inputStream, line); ) {
            // Stop storing the data when it reach the each of data
            if (line.find("</data>") != std::string::npos) {
                foundData = false;
            }
            // Store the whole line as a string
            if (foundData) {
                stringArray.emplace_back(line);
            }
            // Start storing the data (which starts at next line)
            if (line.find("<data encoding=\"csv\">") != std::string::npos) {
                foundData = true;
            }
        }
        inputStream.close();

    } catch(...) {
       std::cout << "Exception caught while loading maze file." << std::endl;
       throw;
    }

    // Parse each line into multiple int and store them in a 2d vector
    int iter = 0;
    std::string delimiter = ",";
    std::string lastString;
    std::string partialString;
    for (auto& oneString : stringArray) {
        result.emplace_back(std::vector<int>());

        size_t pos = 0;

        // After storing the sub string, remove it from the string
        while ((pos = oneString.find(delimiter)) != std::string::npos) {
            partialString = oneString.substr(0, pos);
            result[iter].emplace_back(std::stoi(partialString));
            oneString.erase(0, pos + delimiter.length());
        }
        lastString = oneString;
        iter++;
    }
    // This is needed to store the last string that is not found using the delimiter
    result[iter-1].emplace_back(std::stoi(lastString));

    return result;
};