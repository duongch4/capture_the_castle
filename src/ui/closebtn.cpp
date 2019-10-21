#include "closebtn.hpp"

#include <cmath>

void CloseButton::init(vec2 pos) {
    currIndex = 0;
    transform = Transform {
            { pos.x, pos.y },
            {0.3f,0.6f}
    };
    effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    if (!closebtn_texture.is_valid())
    {
        if(!closebtn_texture.load_from_file(textures_path("ui/CaptureTheCastle_close_icn.png")))
        {
            fprintf(stderr, "Failed to load close button texture");
            return;
        }
    }
    mesh.init(closebtn_texture.width, closebtn_texture.height, closebtn_texture.height, closebtn_texture.height, 0, 0,0);
}

void CloseButton::destroy() {
    effect.release();
    mesh.release();
}

void CloseButton::draw(const mat3 &projection) {
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
    glBindTexture(GL_TEXTURE_2D, closebtn_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&out);
    float color[] = {1, 1, 1};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 CloseButton::get_position() {
    return vec2{transform.position.x, transform.position.y};
}

vec2 CloseButton::get_bounding_box() {
    return {static_cast<float>(fabs(transform.scale.x) * closebtn_texture.width),
            static_cast<float>(fabs(transform.scale.y) * closebtn_texture.height)};
}

bool CloseButton::mouseOnButton(vec2 mouseloc) {
    vec2 button_pos = get_position();
    vec2 button_box = get_bounding_box();
    return  mouseloc.x >= button_pos.x - button_box.x/2 &&
            mouseloc.x <= button_pos.x + button_box.x/2 &&
            mouseloc.y >= button_pos.y - button_box.y/2 &&
            mouseloc.y <= button_pos.y + button_box.y/2;
}
void CloseButton::onHover(bool isHovering) {
    if (isHovering && currIndex == 0) {
        currIndex = 1;
        mesh.updateSprite(closebtn_texture.width, closebtn_texture.height, closebtn_texture.height, closebtn_texture.height, currIndex, 0,0);
    } else if (!isHovering && currIndex == 1) {
        currIndex = 0;
        mesh.updateSprite(closebtn_texture.width, closebtn_texture.height, closebtn_texture.height, closebtn_texture.height, currIndex, 0,0);
    }
}

