//
// Created by Owner on 2019-11-09.
//

#include "mesh_manager.hpp"
#include <iostream>
//MeshManager* MeshManager::inst(0);


MeshManager::~MeshManager() {
    release_all();
    //inst = 0;
}

MeshManager::MeshManager() {
    for (int id = 0; id < MAX_MESH; ++id) {
        availableIds.push(id);
    }
}

int MeshManager::init_mesh(int width, int height) {
    if (activeMeshCount < MAX_MESH) {
        int id = availableIds.front();
        availableIds.pop();
        ++activeMeshCount;

        Mesh mesh{};
        // The position corresponds to the center of the texture.
        float wr = width * 0.4f;
        float hr = height * 0.4f;

        TexturedVertex vertices[4];
        vertices[0].position = { -wr, +hr, -0.01f };
        vertices[0].texcoord = { 0.f, 1.f };
        vertices[1].position = { +wr, +hr, -0.01f };
        vertices[1].texcoord = { 1.f, 1.f, };
        vertices[2].position = { +wr, -hr, -0.01f };
        vertices[2].texcoord = { 1.f, 0.f };
        vertices[3].position = { -wr, -hr, -0.01f };
        vertices[3].texcoord = { 0.f, 0.f };

        // Counterclockwise as it's the default opengl front winding direction.
        uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

        // Clearing errors
        gl_flush_errors();

        // Vertex Buffer creation
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

        // Index Buffer creation
        glGenBuffers(1, &mesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

        // Vertex Array (Container for Vertex + Index buffer)
        glGenVertexArrays(1, &mesh.vao);

        idToMesh.insert({id, mesh});
        if (!gl_has_errors()) {
            return id;
        }
    }
    return -1;
}

int MeshManager::init_mesh(int textureWidth, int textureHeight, float spriteWidth, float spriteHeight, float spriteIndexX,
                       float spriteIndexY, int spriteGap) {
    if (activeMeshCount < MAX_MESH) {
        int id = availableIds.front();
        availableIds.pop();
        ++activeMeshCount;
        Mesh mesh{};

        // The position corresponds to the center of the texture.
        float wr = textureWidth * 0.5f;
        float hr = textureHeight * 0.5f;

        // Normalize the width & height of a sprite in a sprite sheet (including the gap) on the scale of [0..1]
        float spriteNormalWidth = (float) (spriteWidth + spriteGap) / (float) textureWidth;
        float spriteNormalHeight = (float) (spriteHeight + spriteGap) / (float) textureHeight;

        // Normalize the actual width & height of a sprite (excluding the gap) on the scale of [0..1]
        float spriteNormalActWidth = (float) spriteWidth / (float) textureWidth;
        float spriteNormalActHeight = (float) spriteHeight / (float) textureHeight;

        // Calculate the correct texture coordinate based on the width, height, and the index
        // Texture coordinate are in the range of 0 and 1, and that is why we use the normalized width and height
        // The width and height are then multiplied by the index to get the sprite coordinate on the sprite sheet
        TexturedVertex vertices[4];
        vertices[0].position = {-wr, +hr, -0.01f};
        vertices[0].texcoord = {spriteIndexX * spriteNormalWidth,
                                (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight}; // Bottom left
        vertices[1].position = {+wr, +hr, -0.01f};
        vertices[1].texcoord = {(spriteIndexX * spriteNormalWidth) + spriteNormalActWidth,
                                (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight}; // Bottom right
        vertices[2].position = {+wr, -hr, -0.01f};
        vertices[2].texcoord = {(spriteIndexX * spriteNormalWidth) + spriteNormalActWidth,
                                spriteIndexY * spriteNormalHeight}; // Top Right
        vertices[3].position = {-wr, -hr, -0.01f};
        vertices[3].texcoord = {spriteIndexX * spriteNormalWidth, spriteIndexY * spriteNormalHeight}; // Top Left

        // Counterclockwise as it's the default opengl front winding direction.
        uint16_t indices[] = {0, 3, 1, 1, 3, 2};

        // Clearing errors
        gl_flush_errors();

        // Vertex Buffer creation
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

        // Index Buffer creation
        glGenBuffers(1, &mesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

        // Vertex Array (Container for Vertex + Index buffer)
        glGenVertexArrays(1, &mesh.vao);

        idToMesh.insert({id, mesh});
        if (!gl_has_errors()) {
            return id;
        }
    }
    return -1;
}

bool MeshManager::update_sprite(int id, int textureWidth, int textureHeight, int spriteWidth, int spriteHeight,
                                int spriteIndexX, int spriteIndexY, int spriteGap) {
    auto mesh = idToMesh.find(id);
    if (mesh != idToMesh.end()) {
        // The position corresponds to the center of the texture.
        float wr = textureWidth * 0.5f;
        float hr = textureHeight * 0.5f;

        // Normalize the width & height of a sprite in a sprite sheet (including the gap) on the scale of [0..1]
        float spriteNormalWidth = (float)(spriteWidth + spriteGap) / (float)textureWidth;
        float spriteNormalHeight = (float)(spriteHeight + spriteGap) / (float)textureHeight;

        // Normalize the actual width & height of a sprite (excluding the gap) on the scale of [0..1]
        float spriteNormalActWidth = (float)spriteWidth / (float)textureWidth;
        float spriteNormalActHeight = (float)spriteHeight / (float)textureHeight;

        // Calculate the correct texture coordinate based on the width, height, and the index
        // Texture coordinate are in the range of 0 and 1, and that is why we use the normalized width and height
        // The width and height are then multiplied by the index to get the sprite coordinate on the sprite sheet
        TexturedVertex vertices[4];
        vertices[0].position = { -wr, +hr, -0.01f };
        vertices[0].texcoord = { spriteIndexX * spriteNormalWidth, (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight }; // Bottom left
        vertices[1].position = { +wr, +hr, -0.01f };
        vertices[1].texcoord = { (spriteIndexX * spriteNormalWidth) + spriteNormalActWidth, (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight }; // Bottom right
        vertices[2].position = { +wr, -hr, -0.01f };
        vertices[2].texcoord = { (spriteIndexX * spriteNormalWidth) + spriteNormalActWidth, spriteIndexY * spriteNormalHeight }; // Top Right
        vertices[3].position = { -wr, -hr, -0.01f };
        vertices[3].texcoord = { spriteIndexX * spriteNormalWidth, spriteIndexY * spriteNormalHeight }; // Top Left

        // Clearing errors
        gl_flush_errors();

        // Bind the vertex buffer object and update the data (mainly texture coordinate)
        glBindBuffer(GL_ARRAY_BUFFER, mesh->second.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

        return !gl_has_errors();
    }
    return false;
}

void MeshManager::release_all() {
    auto i = idToMesh.begin();
    while(i != idToMesh.end()) {
        glDeleteBuffers(1, &(i->second.vbo));
        glDeleteBuffers(1, &(i->second.ibo));
        glDeleteVertexArrays(1, &(i->second.vao));
        i = idToMesh.erase(i);
    }
    idToMesh.clear();
}

bool MeshManager::release(int id) {
    bool result = true;
    auto mesh = idToMesh.find(id);
    if(mesh != idToMesh.end())
    {
        glDeleteBuffers(1, &(mesh->second.vbo));
        glDeleteBuffers(1, &(mesh->second.ibo));
        glDeleteVertexArrays(1, &(mesh->second.vao));
        idToMesh.erase(mesh);
    }
    else
    {
        result = false;
    }
    return result;
}

void MeshManager::bindVAO(int id) {
    auto mesh = idToMesh.find(id);
    if (mesh != idToMesh.end()) {
        glBindVertexArray(mesh->second.vao);
    }
}

void MeshManager::bindVBO(int id) {
    auto mesh = idToMesh.find(id);
    if (mesh != idToMesh.end()) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->second.vbo);
    }
}

void MeshManager::bindIBO(int id) {
    auto mesh = idToMesh.find(id);
    if (mesh != idToMesh.end()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->second.ibo);
    }
}
