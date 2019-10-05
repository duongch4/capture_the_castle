//
// Created by Shizuko A. on 2019-09-29.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_SOLDIER_HPP
#define CAPTURE_THE_CASTLE_SOLDIER_HPP


#include "common.hpp"
#include "tile.hpp"

class Bandit : public Entity
{
    static Texture bandit_texture;

public:
    Bandit();
    ~Bandit();

    bool init() override;
    void destroy() override;
    void update(float ms) override;
    void draw(const mat3& projection) override;

    vec2 get_position();
    void set_position(vec2 pos);
    void set_direction(vec2 dir);
    vec2 get_bounding_box() const;
    vec2 get_direction();
    bool collides_with_tile(const Tile& tile);
    void handle_wall_collision();
};


#endif //CAPTURE_THE_CASTLE_SOLDIER_HPP
