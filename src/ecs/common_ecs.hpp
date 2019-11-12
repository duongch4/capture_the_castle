//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_COMMON_ECS_HPP
#define CAPTURE_THE_CASTLE_COMMON_ECS_HPP


#include <cstdint>
#include <bitset>
#include <set>
#include <functional>

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 2000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

struct System
{
public:
    std::set<Entity> entities;

    virtual void reset() = 0;
};

// input
enum class InputKeys
{
    UP, DOWN, RIGHT, LEFT,
    W, A, S, D,
    R, Q, SLASH,
    DEFAULT
};

// button actions
enum class ButtonActions
{
    CLOSE,
    MAIN,
    RESTART,
    QUIT,
    HOWTOPLAY,
    LOAD,
    NONE
};

class Event
{
protected:
    virtual ~Event() {}
};

#endif //CAPTURE_THE_CASTLE_COMMON_ECS_HPP
