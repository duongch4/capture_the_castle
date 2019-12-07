//
// Created by Tianyan Zhu on 2019-12-06.
//

#include "effect_manager.hpp"

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

EffectManager& EffectManager::instance() {
    static EffectManager instance;
    return instance;
}

EffectManager::EffectManager() {
    for(int id = 0; id < MAX_EFFECT; ++id) {
        availableIds.push(id);
    }
}

EffectManager::~EffectManager() {
}

bool EffectManager::load_from_file(EffectComponent& ec, bool overwrite) {
    if (ec.fs_name == nullptr || ec.vs_name == nullptr)
        return false;

    if (ec.id != -1 && idToEffect.find(ec.id) != idToEffect.end() && overwrite) {
        release_effect_by_id(ec.id);
    } else if (!overwrite) {
        ec.id = availableIds.front();
        availableIds.pop();
        ++activeEffectCount;
    }

    gl_flush_errors();

    Effect new_effect{};

    // Opening files
    std::ifstream vs_is(ec.vs_name);
    std::ifstream fs_is(ec.fs_name);

    if (!vs_is.good() || !fs_is.good())
    {
        fprintf(stderr, "Failed to load shader files %s, %s", ec.vs_name, ec.fs_name);
        return false;
    }

    // Reading sources
    std::stringstream vs_ss, fs_ss;
    vs_ss << vs_is.rdbuf();
    fs_ss << fs_is.rdbuf();
    std::string vs_str = vs_ss.str();
    std::string fs_str = fs_ss.str();
    const char* vs_src = vs_str.c_str();
    const char* fs_src = fs_str.c_str();
    GLsizei vs_len = (GLsizei)vs_str.size();
    GLsizei fs_len = (GLsizei)fs_str.size();

    new_effect.vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(new_effect.vertex, 1, &vs_src, &vs_len);
    new_effect.fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(new_effect.fragment, 1, &fs_src, &fs_len);

    // Compiling
    // Shaders already delete if compilation fails
    if (!gl_compile_shader(new_effect.vertex))
        return false;

    if (!gl_compile_shader(new_effect.fragment))
    {
        glDeleteShader(new_effect.vertex);
        return false;
    }

    // Linking
    new_effect.program = glCreateProgram();
    glAttachShader(new_effect.program, new_effect.vertex);
    glAttachShader(new_effect.program, new_effect.fragment);
    glLinkProgram(new_effect.program);
    {
        GLint is_linked = 0;
        glGetProgramiv(new_effect.program, GL_LINK_STATUS, &is_linked);
        if (is_linked == GL_FALSE)
        {
            GLint log_len;
            glGetProgramiv(new_effect.program, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log(log_len);
            glGetProgramInfoLog(new_effect.program, log_len, &log_len, log.data());

            release_effect(new_effect);
            fprintf(stderr, "Link error: %s", log.data());
            return false;
        }
    }

    if (gl_has_errors())
    {
        release_effect(new_effect);
        fprintf(stderr, "OpenGL errors occured while compiling Effect");
        return false;
    }
    idToEffect[ec.id] = new_effect;
    return true;
}

bool EffectManager::release_effect_by_id(int id) {
    bool result = true;
    auto effect = idToEffect.find(id);
    if (effect != idToEffect.end())
    {
        release_effect(effect->second);
    } else {
        result = false;
    }
    return result;
}

void EffectManager::release_effect(Effect& e) {
    glDeleteProgram(e.program);
    glDeleteShader(e.vertex);
    glDeleteShader(e.fragment);
}

bool EffectManager::release_all() {
    auto i = idToEffect.begin();
    while(i != idToEffect.end()) {
        release_effect(i->second);
        i = idToEffect.erase(i);
    }
    idToEffect.clear();
}
