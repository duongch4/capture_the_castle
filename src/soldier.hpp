//
// Created by Shizuko A. on 2019-09-29.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_SOLDIER_HPP
#define CAPTURE_THE_CASTLE_SOLDIER_HPP


#include "common.hpp"

class Soldier : public Entity
{
    static Texture soldier_texture;

public:
    Soldier(Team team);
    ~Soldier();

    bool init() override;
    void destroy() override;
    void update(float ms) override;
    void draw(const mat3& projection) override;

private:
    bool is_texture_loaded(const char* path);
	vec2 spriteNum;
	vec2 spriteSize;
};


#endif //CAPTURE_THE_CASTLE_SOLDIER_HPP
