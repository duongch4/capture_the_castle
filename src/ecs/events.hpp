//
// Created by Owner on 2019-10-14.
//

#ifndef CAPTURE_THE_CASTLE_EVENTS_HPP
#define CAPTURE_THE_CASTLE_EVENTS_HPP

#include "common_ecs.hpp"
#include "tile.hpp"

struct InputKeyEvent: public Event {
    InputKeyEvent(InputKeys key) {
        this->key = key;
    }
    ~InputKeyEvent() = default;
    InputKeys key;
};

struct KeyReleaseEvent: public Event {
    KeyReleaseEvent(InputKeys key) {
        this->keyReleased = key;
    }
    ~KeyReleaseEvent() = default;
    InputKeys keyReleased;
};

struct CollisionEvent: public Event{
    CollisionEvent(Entity entity1, Entity entity2){
        this->e1 = entity1;
        this->e2 = entity2;
    }
    ~CollisionEvent() = default;
    Entity e1;
    Entity e2;
};

struct BoxCollisionEvent: public Event{
    BoxCollisionEvent(Entity entity, Tile tile, CollisionResponse collisionResponse) {
        this->e = entity;
        this->tile = tile;
        this-> collisionResponse = collisionResponse;
    }
    ~BoxCollisionEvent() = default;
    Entity e;
    Tile tile;
    CollisionResponse collisionResponse;
};


struct WinEvent: public Event{
    WinEvent(Entity player){
        this->player = player;
    }
    ~WinEvent() = default;
    Entity player;
};

#endif //CAPTURE_THE_CASTLE_EVENTS_HPP
