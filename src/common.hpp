#pragma once

// stlib
#include <fstream> // stdout, stderr..

// glfw
//#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
#include <sstream>

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get definition of PROJECT_SOURCE_DIR from:
#include "project_path.hpp"

#define shader_path(name) PROJECT_SOURCE_DIR "./shaders/" name

#define data_path PROJECT_SOURCE_DIR "./data"
#define textures_path(name)  data_path "/textures/" name
#define audio_path(name) data_path  "/audio/" name
#define mesh_path(name) data_path  "/meshes/" name
#define power_up_path(name) data_path "/textures/power_ups/" name
#define maze_path(name) data_path  "/maze/" name
#define font_path(name) data_path "/fonts/" name
#define flag_path(name) data_path "/textures/capture_the_flag/" name 

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3 mul(const mat3& l, const mat3& r);
vec2 mul(vec2 a, float b);
vec3 mul(mat3 m, vec3 v);
vec2 normalize(vec2 v);
vec2 add(vec2 a, vec2 b);
vec2 sub(vec2 a, vec2 b);
vec2 to_vec2(vec3 v);
float sq_len(vec2 a);
float len(vec2 a);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

enum class TeamType{
    PLAYER1,
    PLAYER2,
    BANDIT,
    ITEM
};

enum class MazeRegion {
	PLAYER1,
	PLAYER2,
	BANDIT
};

enum class CollisionLayer
{
    PLAYER1 = 1,    // bit 0
    PLAYER2 = 2,
    Enemy = 3,// bit 1
    Item = 4,
    Tile = 5,        // bit 2
    Castle = 6

};

enum class ItemType
{
    BOMB,
    SHIELD,
    None
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & player.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

struct CollisionResponse {bool up, down, left, right;};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id = 0;
	GLuint depth_render_buffer_id;
	int width;
	int height;
	
	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	bool is_valid()const; // True if texture is valid
	bool create_from_screen(GLFWwindow const * const window); // Screen texture
};

template <typename T> std::string to_string(const T& t) {
    std::ostringstream os;
    os<<t;
    return os.str();
}