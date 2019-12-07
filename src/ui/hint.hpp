//
// Created by Tianyan Zhu on 2019-12-06.
//

#ifndef CAPTURE_THE_CASTLE_HINT_HPP
#define CAPTURE_THE_CASTLE_HINT_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

#include <common.hpp>
#include <map>
#include <components.hpp>
#include <SDL_mixer.h>
#include <mesh_manager.hpp>


class Hint {
public:
    bool init(vec2 screen_size);
    void draw(const mat3& projection);
    void destroy();

private:
    bool init_text();
    void draw_text(const mat3& projection);


    FT_Library ft;
    FT_Face timer_face;

    int face_height;
    unsigned int max_text_width = 0;
    vec3 text_colour;
    Mesh text_mesh{};
    Effect text_effect;

    MeshComponent clock_mesh{};
    Effect clock_effect{};
    Transform clock_transform{};
    Texture clock_texture;

    std::map<GLchar, Character> characters;
    mat3 out{};
};


#endif //CAPTURE_THE_CASTLE_HINT_HPP
