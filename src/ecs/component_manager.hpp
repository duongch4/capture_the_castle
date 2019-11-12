//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_COMPONENT_MANAGER_HPP
#define CAPTURE_THE_CASTLE_COMPONENT_MANAGER_HPP


#include <unordered_map>
#include <memory>
#include <cassert>
#include "common_ecs.hpp"
#include "component_array.hpp"

class ComponentManager {
public:
    template<typename T>
    void registerComponent() {
        const char* typeName = typeid(T).name();
        assert(componentTypes.find(typeName) == componentTypes.end() && "Component type already registered");

        componentTypes.insert({typeName, nextComponentType});
        componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
        ++nextComponentType;
    }

    template<typename T>
    ComponentType getComponentType() {
        const char* typeName = typeid(T).name();
        printf("%s", typeName);
        assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");
        return componentTypes[typeName];
    }

    template<typename T>
    void addComponent(Entity e, T c) {
        getComponentArray<T>()->insertEC(e, c);
    }

    template<typename T>
    T& getComponent(Entity e) {
        return getComponentArray<T>()->getEC(e);
    }

    template<typename T>
    void removeComponent(Entity e) {
        getComponentArray<T>()->removeEC(e);
    }

    void entityDestroyed(Entity e) {
        for (auto const& ptrAndArray: componentArrays) {
            auto const& componentArray = ptrAndArray.second;
            componentArray->entityDestroyed(e);
        }
    }

    void reset(){
        componentTypes.clear();
        componentArrays.clear();
        nextComponentType = 0;
    }

private:
    std::unordered_map<const char*, ComponentType> componentTypes{};
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};
    ComponentType nextComponentType{};

    template<typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray() {
        const char* typeName = typeid(T).name();
        assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
    }
};

#endif //CAPTURE_THE_CASTLE_COMPONENT_MANAGER_HPP
