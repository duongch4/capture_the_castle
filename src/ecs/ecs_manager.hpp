//
// Created by Owner on 2019-10-12.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_ECS_MANAGER_HPP
#define CAPTURE_THE_CASTLE_ECS_MANAGER_HPP


#include <memory>
#include "common_ecs.hpp"
#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"
#include "event_manager.hpp"

class ECSManager {
public:
    void init() {
        entityManager = std::make_unique<EntityManager>();
        componentManager = std::make_unique<ComponentManager>();
        systemManager = std::make_unique<SystemManager>();
        eventManager = std::make_unique<EventManager>();
    };

    // entity
    Entity createEntity() {
        return entityManager->createEntity();
    };

    void destroyEntity(Entity e) {
        entityManager->destroyEntity(e);
        componentManager->entityDestroyed(e);
        systemManager->entityDestroyed(e);
    };

    // component
    template<typename T>
    void registerComponent() {
        componentManager->registerComponent<T>();
    };

    template<typename T>
    void addComponent(Entity e, T c) {
        componentManager->addComponent(e, c);

        auto signature = entityManager->getSignature(e);
        signature.set(componentManager->getComponentType<T>(), true);
        entityManager->setSignature(e, signature);
        systemManager->entitySignatureChanged(e, signature);
    };

    template<typename T>
    void removeComponent(Entity e) {
        componentManager->removeComponent<T>(e);

        auto signature = entityManager->getSignature(e);
        signature.set(componentManager->getComponentType<T>(), false);
        entityManager->setSignature(e, signature);

        systemManager->entitySignatureChanged(e, signature);
    };

    template<typename T>
    T& getComponent(Entity e) {
        return componentManager->getComponent<T>(e);
    };

    template<typename T>
    ComponentType getComponentType() {
        return componentManager->getComponentType<T>();
    };

    // systems
    template<typename T>
    std::shared_ptr<T> registerSystem() {
        return systemManager->registerSystem<T>();
    };

    template<typename T>
    void setSystemSignature(Signature s) {
        systemManager->setSignature<T>(s);
    };

    // events
    template<typename T, typename EventType>
    void subscribe(T* instance, void (T::*memberFunction)(EventType*)) {
        eventManager->subscribe<T, EventType>(instance, memberFunction);
    }

    template<typename EventType>
    void publish(EventType* e) {
        eventManager->publish<EventType>(e);
    };

    void reset() {
        entityManager->reset();
        systemManager->reset();
        componentManager->reset();
        eventManager->reset();
    }

private:
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<SystemManager> systemManager;
    std::unique_ptr<EventManager> eventManager;
};


#endif //CAPTURE_THE_CASTLE_ECS_MANAGER_HPP
