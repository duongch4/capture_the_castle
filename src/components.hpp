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
   int id;
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
struct C_Collision{
    CollisionLayer layer;
    float radius;
    vec2 boundingBox;
};

struct ItemComponent{
    bool in_use;
    ItemType itemType;
};

// Team component to specify an entity belongs to a team
struct Team {
    TeamType assigned;
};

struct BanditSpawnComponent {};
struct BanditAiComponent {};
struct ItemBoardComponent {};
struct PlaceableComponent{};

struct PlayerInputControlComponent {};

struct SoldierAiComponent {};

struct CurveMotionComponent{
    vec2 p0;
    vec2 p1;
    vec2 p2;
    vec2 p3;
};

#endif //CAPTURE_THE_CASTLE_COMPONENTS_HPP
