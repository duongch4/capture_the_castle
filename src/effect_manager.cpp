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
    Effect new_effect{};
    bool loaded = new_effect.load_from_file(ec.vs_name, ec.fs_name);
    if (loaded) {
        idToEffect[ec.id] = new_effect;
    }
    return loaded;
}

bool EffectManager::release_effect_by_id(int id) {
    bool result = true;
    auto effect = idToEffect.find(id);
    if (effect != idToEffect.end())
    {
        effect->second.release();
    } else {
        result = false;
    }
    return result;
}

bool EffectManager::release_all() {
    auto i = idToEffect.begin();
    while(i != idToEffect.end()) {
        i->second.release();
        i = idToEffect.erase(i);
    }
    idToEffect.clear();
    return true;
}

GLuint EffectManager::get_program(int id) {
    auto effect = idToEffect.find(id);
    if (effect != idToEffect.end())
    {
        return effect->second.program;
    }
    else
    {
        return -1;
    }
}

bool Effect::load_from_file(const char *vs_path, const char *fs_path) {
    gl_flush_errors();

    // Opening files
    std::ifstream vs_is(vs_path);
    std::ifstream fs_is(fs_path);

    if (!vs_is.good() || !fs_is.good())
    {
        fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
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

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs_src, &vs_len);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs_src, &fs_len);

    // Compiling
    // Shaders already delete if compilation fails
    if (!gl_compile_shader(vertex))
        return false;

    if (!gl_compile_shader(fragment))
    {
        glDeleteShader(vertex);
        return false;
    }

    // Linking
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    {
        GLint is_linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
        if (is_linked == GL_FALSE)
        {
            GLint log_len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log(log_len);
            glGetProgramInfoLog(program, log_len, &log_len, log.data());

            release();
            fprintf(stderr, "Link error: %s", log.data());
            return false;
        }
    }

    if (gl_has_errors())
    {
        release();
        fprintf(stderr, "OpenGL errors occured while compiling Effect");
        return false;
    }

    return true;
}

void Effect::release() {
    glDeleteProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
