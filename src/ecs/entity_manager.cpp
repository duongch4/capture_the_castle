//
// Created by Owner on 2019-10-12.
//

#include <assert.h>
#include <iostream>
#include "entity_manager.hpp"

EntityManager::EntityManager()
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
    {
        availableEntities.push(e);
    }
}

Entity EntityManager::createEntity()
{
    std::cout<<activeEntityCount<<std::endl;
    assert(activeEntityCount < MAX_ENTITIES && "Too many entities");

    Entity id = availableEntities.front();
    availableEntities.pop();
    ++activeEntityCount;
    return id;
}

void EntityManager::destroyEntity(Entity e) {
    assert(e < MAX_ENTITIES && "Entity out of range");

    signatures[e].reset();
    availableEntities.push(e);
    --activeEntityCount;
}

void EntityManager::setSignature(Entity e, Signature s) {
    assert(e < MAX_ENTITIES && "Entity out of range");

    signatures[e] = s;
}

Signature EntityManager::getSignature(Entity e) {
    assert(e < MAX_ENTITIES && "Entity out of range");

    return signatures[e];
}

void EntityManager::reset() {
    std::queue<Entity> empty;
    std::swap(empty, availableEntities);
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
    {
        availableEntities.push(e);
    }
    activeEntityCount = 0;
    signatures.fill(0);
}





