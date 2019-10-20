//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_COMPONENT_ARRAY_HPP
#define CAPTURE_THE_CASTLE_COMPONENT_ARRAY_HPP

#include <array>
#include <unordered_map>
#include "common_ecs.hpp"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity e) = 0;
};

template<typename T>
class ComponentArray: public IComponentArray {
public:
    void insertEC(Entity e, T c)
    {
        assert(entityToIndexMap.find(e) == entityToIndexMap.end() && "Component already added");

        size_t newIndex = componentArraySize;
        entityToIndexMap[e] = newIndex;
        indexToEntityMap[newIndex] = e;
        componentArray[newIndex] = c;
        ++componentArraySize;
    }
    void removeEC(Entity e)
    {
        assert(entityToIndexMap.find(e) != entityToIndexMap.end() && "Component not found");

        size_t indexToRemove = entityToIndexMap[e];
        size_t indexOfLastComponent = componentArraySize - 1;
        componentArray[indexToRemove] = componentArray[indexOfLastComponent];
        Entity entityOfLastComponent = indexToEntityMap[indexOfLastComponent];
        entityToIndexMap[entityOfLastComponent] = indexToRemove;
        indexToEntityMap[indexToRemove] = entityOfLastComponent;

        entityToIndexMap.erase(e);
        indexToEntityMap.erase(indexOfLastComponent);

        --componentArraySize;
    }
    T& getEC(Entity e)
    {
        assert(entityToIndexMap.find(e) != entityToIndexMap.end() && "Component not found");

        return componentArray[entityToIndexMap[e]];
    }
    void entityDestroyed(Entity e)
    {
        if (entityToIndexMap.find(e) != entityToIndexMap.end())
        {
            removeEC(e);
        }
    }

private:
    std::array<T, MAX_ENTITIES> componentArray;
    std::unordered_map<Entity, size_t> entityToIndexMap;
    std::unordered_map<size_t, Entity> indexToEntityMap;
    size_t componentArraySize = 0;
};

#endif //CAPTURE_THE_CASTLE_COMPONENT_ARRAY_HPP
