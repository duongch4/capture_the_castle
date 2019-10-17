//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_COMPONENTS_HPP
#define CAPTURE_THE_CASTLE_COMPONENTS_HPP

#include "common.hpp"

struct Sprite {
    char* texture_name;
    int width, height = 0;
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer.
struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    bool init(int width, int height);
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
