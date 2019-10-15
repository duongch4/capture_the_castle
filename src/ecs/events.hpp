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
#endif //CAPTURE_THE_CASTLE_EVENTS_HPP
