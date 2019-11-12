//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_SYSTEM_MANAGER_HPP
#define CAPTURE_THE_CASTLE_SYSTEM_MANAGER_HPP


#include <memory>
#include <unordered_map>
#include "ecs/common_ecs.hpp"

class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> registerSystem() {
        const char* typeName = typeid(T).name();
        assert(systems.find(typeName) == systems.end() && "Registering systems more than once.");

        auto system = std::make_shared<T>();
        systems.insert({typeName, system});
        return system;
    }

    template<typename T>
    void setSignature(Signature s) {
        const char* typeName = typeid(T).name();
        assert(systems.find(typeName) != systems.end() && "System used before registered.");
        signatures.insert({typeName, s});
    }

    void entityDestroyed(Entity e) {
        for (auto const& ptrAndSystem : systems)
        {
            auto const& system = ptrAndSystem.second;
            system->entities.erase(e);
        }
    }

    void entitySignatureChanged(Entity e, Signature newSignature) {
        for (auto const& ptrAndSystem : systems)
        {
            auto const& type = ptrAndSystem.first;
            auto const& system = ptrAndSystem.second;
            auto const& systemSignature = signatures[type];

            if ((newSignature & systemSignature) == systemSignature){
                system->entities.insert(e);
            } else {
                system->entities.erase(e);
            }
        }
    }

    void reset() {
        signatures.clear();
        auto it = systems.begin();
        while(it != systems.end()) {
            it->second->reset();
            it++;
        }
        systems.clear();
    }

private:
    std::unordered_map<const char*, Signature> signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};


#endif //CAPTURE_THE_CASTLE_SYSTEM_MANAGER_HPP
