//
// Created by Owner on 2019-11-29.
//

#ifndef CAPTURE_THE_CASTLE_COUNTDOWN_TIMER_HPP
#define CAPTURE_THE_CASTLE_COUNTDOWN_TIMER_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

#include <common.hpp>
#include <map>
#include <components.hpp>

struct ivec2 {
    int x, y;
};

struct uivec2 {
    unsigned int x, y;
};

struct Character {
    GLuint texture_id;
    uivec2 size;
    ivec2 bearing;
    GLuint advance;
};

class CountdownTimer {
public:
    bool init(vec2 screen_size);
    void start_timer(float seconds);
    void update(float elapsed_time);
    bool check_times_up();
    void draw(const mat3& projection);
    void destroy();

private:
    bool init_clock(vec2 screen_size);
    bool init_text();
    void reset_timer();
    void draw_text(const mat3& projection);

    float remaining_time;
    bool timer_active;

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


#endif //CAPTURE_THE_CASTLE_COUNTDOWN_TIMER_HPP
