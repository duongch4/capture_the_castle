//
// Created by Owner on 2019-11-09.
//

#ifndef CAPTURE_THE_CASTLE_MESH_MANAGER_HPP
#define CAPTURE_THE_CASTLE_MESH_MANAGER_HPP

#include <map>
#include <queue>
#include "components.hpp"

struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
};

const int MAX_MESH = 2000;

class MeshManager {
public:
    static MeshManager* instance();
    ~MeshManager();
    int init_mesh(int width, int height);
    int init_mesh(int textureWidth, int textureHeight, float spriteWidth,
                   float spriteHeight, float spriteIndexX, float spriteIndexY, int spriteGap);
    bool update_sprite(int id, int textureWidth, int textureHeight, int spriteWidth, int spriteHeight, int spriteIndexX, int spriteIndexY, int spriteGap);
    void bindVAO(int id);
    void bindVBO(int id);
    void bindIBO(int id);

    bool release(int id);
    void release_all();

protected:
    MeshManager();
    static MeshManager* inst;
    std::queue<int> availableIds{};
    std::map<int, Mesh> idToMesh;
    uint32_t activeMeshCount{};
};


#endif //CAPTURE_THE_CASTLE_MESH_MANAGER_HPP
