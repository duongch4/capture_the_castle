//
// Created by Owner on 2019-10-12.
//


#include <iostream>
#include "render_system.hpp"

extern ECSManager ecsManager;

void SpriteRenderSystem::init() {
}

namespace
{
    bool gl_compile_shader(GLuint shader)
    {
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint log_len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log(log_len);
            glGetShaderInfoLog(shader, log_len, &log_len, log.data());
            glDeleteShader(shader);

            fprintf(stderr, "GLSL: %s", log.data());
            return false;
        }

        return true;
    }
}

void SpriteRenderSystem::draw(mat3 projection) {
    for (auto const& e : entities) {
        auto const& transform = ecsManager.getComponent<Transform>(e);
        auto const& sprite = ecsManager.getComponent<Sprite>(e);
        auto const& mesh = ecsManager.getComponent<Mesh>(e);
        auto const& effect = ecsManager.getComponent<Effect>(e);

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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        // Getting uniform locations
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
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) 0);
        glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) sizeof(vec3));

        // Enabling and binding texture to slot 0
        glActiveTexture(GL_TEXTURE0);
        TextureManager::instance()->bind_texture(sprite.texture_name);

        // Setting uniform values to the currently bound program
        glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *) &out);

        // Setting color
        float color[] = {1.f, 1.f, 1.f};
        glUniform3fv(color_uloc, 1, color);
        glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *) &projection);

        // Drawing!
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    }
}
