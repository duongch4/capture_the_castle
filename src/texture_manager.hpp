//
// Created by Owner on 2019-10-14.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_TEXTURE_MANAGER_HPP
#define CAPTURE_THE_CASTLE_TEXTURE_MANAGER_HPP


#include <map>
#include <glcorearb.h>
#include "components.hpp"

class TextureManager {

public:
    static TextureManager* instance();
    ~TextureManager();

    bool load_from_file(Sprite& sprite, bool overwrite = false);
    bool unload_texture(const std::string tex_name);
    bool bind_texture(const std::string tex_name);
    void unload_all_textures();

protected:
    TextureManager();
    static TextureManager* inst;
    std::string textures_path = data_path "/textures/";
    std::map<std::string, GLuint> nameToId;
};


#endif //CAPTURE_THE_CASTLE_TEXTURE_MANAGER_HPP
