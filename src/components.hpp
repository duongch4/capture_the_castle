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
};

struct EffectComponent {
   const char* vs_name;
   const char* fs_name;
   int id = -1;
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
