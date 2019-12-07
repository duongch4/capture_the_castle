//
// Created by Owner on 2019-10-12.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_COMPONENTS_HPP
#define CAPTURE_THE_CASTLE_COMPONENTS_HPP

#include "common.hpp"

struct Sprite {
    const char* texture_name;
    int width, height = 0;
	// storing index and size of this sprite in sprite sheet
	vec2 sprite_index = {0, 0};
	vec2 sprite_size = {0, 0};
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer.
struct MeshComponent {
    int id;
    //bool init(int width, int height);
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
//
//	bool init(
//		int textureWidth, int textureHeight, float spriteWidth, float spriteHeight,
//		float spriteIndexX, float spriteIndexY, int spriteGap
//	);
//
//	// Very similar to the init function, but this will only update the vertex
//	// buffer object instead of generating a new vertex buffer object.
//	bool updateSprite(int textureWidth, int textureHeight, int spriteWidth, int spriteHeight,
//		int spriteIndexX, int spriteIndexY, int spriteGap);
//
//  void release();

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
    vec2 init_position;
    vec2 scale;
    vec2 old_position;
};

// Collision component of an entity handles collision testing and collision handling
struct C_Collision {
    CollisionLayer layer;
    float radius;
    vec2 boundingBox;
    int wait_count = 0;
};

struct ItemComponent {
    bool in_use;
    ItemType itemType;
};

// Team component to specify an entity belongs to a team
struct Team {
    TeamType assigned;
};

struct BanditSpawnComponent {
	BanditType type;
};

struct BanditAiComponent {
	BanditState	state;
	size_t idle_time;
	size_t chase_time;
	size_t patrol_time;
	vec2 prev_dir;
};

struct ItemBoardComponent {};
struct PlaceableComponent {};

struct PlayerInputControlComponent {};

struct SoldierAiComponent {
	SoldierState state;
	size_t idle_time;
	size_t patrol_time;
	vec2 prev_dir;
};

struct CurveMotionComponent {
    vec2 p0;
    vec2 p1;
    vec2 p2;
    vec2 p3;
};

struct RainComponent {
	size_t test;
};

bool intersect_line_point(float x1, float y1, float x2, float y2, float px, float py);
bool intersect_point_circle(float px, float py, float cx, float cy, float r);
bool intersect_line_circle(float x1, float y1, float x2, float y2, float cx, float cy, float r);
bool intersect_line_line(
	float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4
);
bool intersect_line_rect(
	float x1, float y1, float x2, float y2,
	float rx, float ry, float rw, float rh
);

float dist(float x1, float y1, float x2, float y2);
bool intersect_circle_circle(const vec2& c1, const vec2& c2, const float& r1, const float& r2);

#endif //CAPTURE_THE_CASTLE_COMPONENTS_HPP
