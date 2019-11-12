//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_ENTITY_MANAGER_HPP
#define CAPTURE_THE_CASTLE_ENTITY_MANAGER_HPP

#include <queue>
#include <array>
#include "common_ecs.hpp"

class EntityManager {

public:
    EntityManager();
    Entity createEntity();
    void destroyEntity(Entity e);
    void setSignature(Entity e, Signature s);
    Signature getSignature(Entity e);
    void reset();

private:
    std::queue<Entity> availableEntities{};
    std::array<Signature, MAX_ENTITIES> signatures{};
    uint32_t activeEntityCount{};
};


#endif //CAPTURE_THE_CASTLE_ENTITY_MANAGER_HPP
