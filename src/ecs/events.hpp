//
// Created by Owner on 2019-10-14.
//

#ifndef CAPTURE_THE_CASTLE_EVENTS_HPP
#define CAPTURE_THE_CASTLE_EVENTS_HPP

#include "common_ecs.hpp"

struct InputKeyEvent: public Event {
    InputKeyEvent(InputKeys key) {
        this->key = key;
    }
    InputKeys key;
};

struct KeyReleaseEvent: public Event {
    KeyReleaseEvent(InputKeys key) {
        this->keyReleased = key;
    }
    InputKeys keyReleased;
};

struct CollisionEvent: public Event{
    CollisionEvent(Entity entity1, Entity entity2){
        this->e1 = entity1;
        this->e2 = entity2;
    }
    Entity e1;
    Entity e2;
};

#endif //CAPTURE_THE_CASTLE_EVENTS_HPP
