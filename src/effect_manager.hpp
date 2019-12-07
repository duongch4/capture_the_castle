//
// Created by Tianyan Zhu on 2019-12-06.
//

#ifndef CAPTURE_THE_CASTLE_EFFECT_MANAGER_HPP
#define CAPTURE_THE_CASTLE_EFFECT_MANAGER_HPP

#include <queue>
#include <map>
#include "components.hpp"

// Effect component of Entity for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect {
    GLuint vertex;
    GLuint fragment;
    GLuint program;
    bool load_from_file(const char* vs_path, const char* fs_path);
    void release();
};

const int MAX_EFFECT = 2000;

class EffectManager {
public:
    static EffectManager &instance();

    ~EffectManager();

    bool load_from_file(EffectComponent& ec, bool overwrite = false);

    bool release_effect_by_id(int id);

    bool release_all();

    GLuint get_program(int id);

protected:
    EffectManager();

    std::map<int, Effect> idToEffect;
    std::queue<int> availableIds{};
    uint32_t activeEffectCount{};
};


#endif //CAPTURE_THE_CASTLE_EFFECT_MANAGER_HPP
