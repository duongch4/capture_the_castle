//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_COMPONENTS_HPP
#define CAPTURE_THE_CASTLE_COMPONENTS_HPP

#include "common.hpp"

struct Sprite {
    char* texture_name;
    int width, height = 0;
	// sotring index and size of this sprite in sprite sheet
	vec2 sprite_index = {0, 0};
	vec2 sprite_size = {0, 0};
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer.
struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    bool init(int width, int height);
	/**
	* Initialize the mesh with a sprite in a sprite sheet
	*
	* The previous init function only works on a single sprite,
	* this init function takes additional parameters to get
	* the correct sprite on a sprite sheet.
	*
	* @param textureWidth  The width of the entire texture/spritesheet, in pixels.
	* @param textureHeight The height of the entire texture/spritesheet, in pixels.
	* @param spriteWidth   The width of a single sprite in the texture/spritesheet, in pixels.
	* @param spriteHeight  The height of a single sprite in the texture/spritesheet, in pixels.
	* @param spriteIndexX  The x index of the sprite in the texture/spritesheet,
	*		                top left is 0 and going right will increase the value
	* @param spriteIndexY  The y index of the sprite in the texture/spritesheet,
	*					    top left is 0 and going down will increase the value
	* @param spriteGap     The gap/padding between each sprite in the texture/spritesheet, in pixels.
	*                      Enter 0 if there is no gap in the sprite sheet
	* @return boolean      True if the mesh is created successfully, false otherwise.
	*/
	bool init(int textureWidth, int textureHeight, int spriteWidth, int spriteHeight,
		int spriteIndexX, int spriteIndexY, int spriteGap);
};

// Effect component of Entity for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect {
    GLuint vertex;
    GLuint fragment;
    GLuint program;
    bool load_from_file(const char* vs_path, const char* fs_path);
    void release();
};

// All data relevant to the motion of the salmon.
struct Motion {
    vec2 direction;
    float speed;
};

// Transform component handles transformations passed to the Vertex shader.
// gl Immediate mode equivalent, see the Rendering and Transformations section in the
// specification pdf.
struct Transform {
    vec2 position;
    vec2 scale;
};
// Collision component of an entity handles collision testing and collision handling
struct Collision {
    vec2 bounding_box;
};

// Team component to specify an entity belongs to a team
struct Team {
    TeamType assigned;
};


struct BanditSpawnComponent {};

struct PlayerInputControlComponent {};

#endif //CAPTURE_THE_CASTLE_COMPONENTS_HPP
