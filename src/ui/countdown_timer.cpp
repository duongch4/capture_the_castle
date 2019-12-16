//
// Created by Owner on 2019-11-29.
//

#include <mesh_manager.hpp>
#include <iostream>
#include "countdown_timer.hpp"

bool CountdownTimer::init(vec2 screen_size) {
    remaining_time = 0;
    timer_active = false;
    characters.clear();
    ticking_sound = Mix_LoadWAV(audio_path("capturethecastle_ticktock.wav"));
    if (ticking_sound == nullptr)
        return false;
    timeout_sound = Mix_LoadWAV(audio_path("capturethecastle_timeout.wav"));
    if (timeout_sound == nullptr)
        return false;
    return init_clock(screen_size) && init_text();
}

void CountdownTimer::start_timer(float seconds) {
    remaining_time = seconds;
    timer_active = true;
    Mix_PlayChannel(-1, ticking_sound, 2);
}

void CountdownTimer::update(float elapsed_time) {
    if (timer_active && remaining_time > 0) {
        remaining_time -= elapsed_time / 1000;
    }
}


bool CountdownTimer::check_times_up() {
   if (timer_active && remaining_time < 0) {
       Mix_PlayChannel(-1, timeout_sound, 0);
       reset_timer();
       return true;
   }
   return false;
}

bool CountdownTimer::init_clock(vec2 screen_size) {
    clock_transform = Transform {
            {screen_size.x / 2, screen_size.y / 2 - 335},
            {screen_size.x / 2, screen_size.y / 2 - 335},
            {0.2f, 0.2f},
            {screen_size.x / 2, screen_size.y / 2}
    };
    clock_effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));

    if (!clock_texture.is_valid())
    {
        if (!clock_texture.load_from_file(textures_path("ui/CaptureTheCastle_timer.png")))
        {
            fprintf(stderr, "Failed to load clock texture!");
        }
    }
    clock_mesh.id = MeshManager::instance().init_mesh(clock_texture.width, clock_texture.height);
    return true;
}

bool CountdownTimer::init_text() {
    face_height = 48;
    text_colour = vec3{ 0.86f, 0.62f, 0.156f };
    if (FT_Init_FreeType(&ft))
        return false;
    if (FT_New_Face(ft, font_path("coopbl.ttf"), 0, &timer_face))
        return false;

    FT_Set_Pixel_Sizes(timer_face, 0, face_height);

    // Disabling the default 4-byte alignment restrictions used by OpenGL
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (GLubyte n = 48; n < 58; n++)
    {
        if (FT_Load_Char(timer_face, n, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     timer_face->glyph->bitmap.width,
                     timer_face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     timer_face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
                texture,
                uivec2{timer_face->glyph->bitmap.width, timer_face->glyph->bitmap.rows},
                ivec2{timer_face->glyph->bitmap_left, timer_face->glyph->bitmap_top},
                static_cast<GLuint>(timer_face->glyph->advance.x)
        };
        characters.insert(std::pair<GLchar, Character>(n, character));
        if (character.size.x > max_text_width) {
            max_text_width = character.size.x;
        }
    }
    FT_Done_Face(timer_face);
    FT_Done_FreeType(ft);

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer & Vertex Array creation
    glGenVertexArrays(1, &text_mesh.vao);
    glGenBuffers(1, &text_mesh.vbo);
    glBindVertexArray(text_mesh.vao);
    // Copy the vertex data into the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, text_mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    // Bind to attribute 0 (in_position) as in the vertex shader
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the vertex array to prevent accidental change
    glBindVertexArray(0);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (gl_has_errors())
        return false;

    // Loading shaders
    return text_effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl"));
}

void CountdownTimer::destroy() {
    clock_effect.release();
    MeshManager::instance().release(clock_mesh.id);
    characters.clear();

    glDeleteBuffers(1, &text_mesh.vbo);
    glDeleteBuffers(1, &text_mesh.ibo);
    glDeleteVertexArrays(1, &text_mesh.vao);
    text_effect.release();
    if (ticking_sound != nullptr)
        Mix_FreeChunk(ticking_sound);
    if (timeout_sound != nullptr)
        Mix_FreeChunk(timeout_sound);
}

void CountdownTimer::draw_text(const mat3 &projection) {
    std::string text = to_string(static_cast<int>(remaining_time));
    size_t half_width = text.length() * max_text_width / 2;
    float x = clock_transform.position.x - half_width;
    float y = clock_transform.position.y - 5;
    float scale = 1.0f;
    // Setting shaders
    glUseProgram(text_effect.program);
    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_mesh.vao);

    GLint text_colour_loc = glGetUniformLocation(text_effect.program, "textColour");
    glUniform3f(text_colour_loc, text_colour.x, text_colour.y, text_colour.z);

    GLint projection_uloc = glGetUniformLocation(text_effect.program, "projection");
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Iterate through all characters
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 1.0 },
                { xpos,     ypos,       0.0, 0.0 },
                { xpos + w, ypos,       1.0, 0.0 },

                { xpos,     ypos + h,   0.0, 1.0 },
                { xpos + w, ypos,       1.0, 0.0 },
                { xpos + w, ypos + h,   1.0, 1.0 }
        };

        GLint in_position_loc = glGetAttribLocation(text_effect.program, "in_position");
        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, text_mesh.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // draw text
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CountdownTimer::draw(const mat3 &projection) {
    out = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} };

    // apply translation
    float offset_x = clock_transform.position.x;
    float offset_y = clock_transform.position.y;
    mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset_x, offset_y, 1.f } };
    out = mul(out, T);

    // apply scale
    mat3 S = { {clock_transform.scale.x, 0.f, 0.f },{ 0.f, clock_transform.scale.y, 0.f },{ 0.f, 0.f, 1.f } };
    out = mul(out, S);

    // Setting shaders
    glUseProgram(clock_effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(clock_effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(clock_effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(clock_effect.program, "projection");

    // Setting vertices and indices
    MeshManager::instance().bindVAO(clock_mesh.id);
    MeshManager::instance().bindVBO(clock_mesh.id);
    MeshManager::instance().bindIBO(clock_mesh.id);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(clock_effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(clock_effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)nullptr);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, clock_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&out);
    float color[] = {1, 1, 1};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    draw_text(projection);
}

void CountdownTimer::reset_timer() {
    timer_active = false;
    remaining_time = 0;
}
