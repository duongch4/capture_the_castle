#include "popupbtn.hpp"
#include <string>
#include <cmath>

void PopUpButton::init(vec2 pos, const char* texturePath) {
    currIndex = 0;
    transform = Transform {
            { pos.x, pos.y },
            { pos.x, pos.y},
            {0.5f, 0.25f},
            { pos.x, pos.y }

    };
    effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));

    // Load shared texture
    if (!popupBtnSprite.is_valid())
    {
        if (!popupBtnSprite.load_from_file(texturePath))
        {
            fprintf(stderr, "Failed to load tile texture!");
        }
    }
    mesh.init(popupBtnSprite.width, popupBtnSprite.height, popupBtnSprite.width, popupBtnSprite.height / 2, 0, currIndex, 0);
}

void PopUpButton::destroy() {
    effect.release();
    mesh.release();
}

void PopUpButton::draw(const mat3 &projection) {
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    // begin transform
    out = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} };

    // apply translation
    float offset_x = transform.position.x;
    float offset_y = transform.position.y;
    mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset_x, offset_y, 1.f } };
    out = mul(out, T);

    // apply scale
    mat3 S = { { transform.scale.x, 0.f, 0.f },{ 0.f, transform.scale.y, 0.f },{ 0.f, 0.f, 1.f } };
    out = mul(out, S);

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)nullptr);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, popupBtnSprite.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&out);
    float color[] = {1, 1, 1};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 PopUpButton::get_position() {
    return vec2{transform.position.x, transform.position.y};
}

vec2 PopUpButton::get_bounding_box() {
    return {static_cast<float>(fabs(transform.scale.x) * popupBtnSprite.width),
            static_cast<float>(fabs(transform.scale.y) * popupBtnSprite.height)};
}

bool PopUpButton::mouseOnButton(vec2 mouseloc) {
    vec2 button_pos = get_position();
    vec2 button_box = get_bounding_box();
    return  mouseloc.x >= button_pos.x - button_box.x/2 &&
            mouseloc.x <= button_pos.x + button_box.x/2 &&
            mouseloc.y >= button_pos.y - button_box.y/2 &&
            mouseloc.y <= button_pos.y + button_box.y/2;
}

void PopUpButton::onHover(bool isHovering) {
    if (isHovering && currIndex == 0) {
        currIndex = 1;
        mesh.updateSprite(popupBtnSprite.width, popupBtnSprite.height, popupBtnSprite.width,
                          popupBtnSprite.height / 2, 0, currIndex, 0);
    } else if (!isHovering && currIndex == 1) {
        currIndex = 0;
        mesh.updateSprite(popupBtnSprite.width, popupBtnSprite.height, popupBtnSprite.width,
                          popupBtnSprite.height / 2, 0, currIndex, 0);
    }
}
