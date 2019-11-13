//
// Created by Owner on 2019-10-14.
//

#include <stb_image.h>
#include "texture_manager.hpp"
#include "common.hpp"
#include "components.hpp"

TextureManager* TextureManager::inst(0);

TextureManager* TextureManager::instance()
{
    if(!inst)
        inst = new TextureManager();
    return inst;
}

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
    unload_all_textures();
    inst = 0;
}

bool TextureManager::load_from_file(Sprite& sprite, bool overwrite)
{
    if (sprite.texture_name == nullptr)
        return false;

    // unload the current texture if id already in use
    if(nameToId.find(sprite.texture_name) != nameToId.end() && overwrite) {
        glDeleteTextures(1, &(nameToId[sprite.texture_name]));
    }

    if (!overwrite) {
        GLuint gl_tex_name;
        stbi_uc* data = stbi_load(sprite.texture_name, &sprite.width, &sprite.height, NULL, 4);
        if (data == nullptr)
            return false;

        gl_flush_errors();

        //generate an OpenGL texture ID for this texture
        glGenTextures(1, &gl_tex_name);
        //store the texture ID mapping
        nameToId[sprite.texture_name] = gl_tex_name;
        //bind to the new texture ID
        glBindTexture(GL_TEXTURE_2D, gl_tex_name);
        //store the texture data for OpenGL use
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite.width, sprite.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // free image data
        stbi_image_free(data);
    }
    
    return !gl_has_errors();
}

bool TextureManager::unload_texture(const std::string tex_name)
{
    bool result = true;
    //if this texture ID mapped, unload it's texture, and remove it from the map
    if(nameToId.find(tex_name) != nameToId.end())
    {
        glDeleteTextures(1, &(nameToId[tex_name]));
        nameToId.erase(tex_name);
    }
    else
    {
        result = false;
    }

    return result;
}

bool TextureManager::bind_texture(const std::string tex_name)
{
    bool result(true);
    //if this texture ID mapped, bind it's texture as current
    if(nameToId.find(tex_name) != nameToId.end())
        glBindTexture(GL_TEXTURE_2D, nameToId[tex_name]);
        //otherwise, binding failed
    else
        result = false;

    return result;
}

void TextureManager::unload_all_textures()
{
    auto i = nameToId.begin();
    while(i != nameToId.end()) {
        glDeleteTextures(1, &(i->second));
        i = nameToId.erase(i);
    }
    nameToId.clear();
}
