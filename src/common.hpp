#pragma once

// stlib
#include <fstream> // stdout, stderr..

// glfw
//#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

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
    BANDIT
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
    SHIELD
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

//// An entity boils down to a collection of components,
//// organized by their in-game context (mesh, effect, motion, etc...)
//struct Entity {
//	// projection contains the orthographic projection matrix. As every Entity::draw()
//	// renders itself it needs it to correctly bind it to its shader.
//	virtual bool init() = 0;
//	virtual void draw(const mat3& projection) = 0;
//	virtual void update(float ms) = 0;
//	virtual void destroy() = 0;
//
//protected:
//	// Sprite component of Entity for texture
//	struct SpriteComponent {
//		Texture texture;
//	} sprite;
//
//    // A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
//    // represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
//    // an Index Buffer.
//    struct MeshComponent {
//        GLuint vao;
//        GLuint vbo;
//        GLuint ibo;
//    } mesh;
//
//    // Effect component of Entity for Vertex and Fragment shader, which are then put(linked) together in a
//	// single program that is then bound to the pipeline.
//	struct EffectComponent {
//		GLuint vertex;
//		GLuint fragment;
//		GLuint program;
//
//		bool load_shaders(const char* vs_path, const char* fs_path); // load shaders from files and link into program
//		void release(); // release shaders and program
//	} effect;
//
//	// All data relevant to the motion of the salmon.
//	struct MotionComponent {
//	    vec2 direction;
//		float speed;
//	} motion;
//
//	// Position of an entity
//	struct PositionComponent {
//	    float pos_x;
//	    float pos_y;
//	} position;
//
//	// Scale is used in the bounding box calculations,
//	// and so contextually belongs here (for now).
//	struct PhysicsComponent {
//		vec2 scale;
//	} physics;
//
//	// Transform component handles transformations passed to the Vertex shader.
//	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
//	// specification pdf.
//	struct TransformComponent {
//		mat3 out;
//
//		void begin();
//		void scale(vec2 scale);
//		void translate(vec2 offset);
//		void end();
//	} transform;
//
//	// Collision component of an entity handles collision testing and collision handling
//	struct CollisionComponent {
//        vec2 bounding_box;
//	} collision;
//
//	// Team component to specify an entity belongs to a team
//	struct TeamComponent {
//	    Team assigned;
//    }team;
//};
