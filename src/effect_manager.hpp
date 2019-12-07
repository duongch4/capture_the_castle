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
};

const int MAX_EFFECT = 2000;

class EffectManager {
public:
    static EffectManager &instance();

    ~EffectManager();

    bool load_from_file(EffectComponent& ec, bool overwrite);

    bool release_effect_by_id(int id);

    void release_effect(Effect& e);

    bool release_all();

protected:
    EffectManager();

    std::string effect_path = data_path "/effect/";
    std::map<int, Effect> idToEffect;
    std::queue<int> availableIds{};
    uint32_t activeEffectCount{};
};


#endif //CAPTURE_THE_CASTLE_EFFECT_MANAGER_HPP
