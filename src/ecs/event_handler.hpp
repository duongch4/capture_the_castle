//
// Created by Owner on 2019-10-14.
//
#pragma once

#ifndef CAPTURE_THE_CASTLE_EVENT_HANDLER_HPP
#define CAPTURE_THE_CASTLE_EVENT_HANDLER_HPP

#include "common_ecs.hpp"

// This is the interface for MemberFunctionHandler that each specialization will use
class IEventHandler {
public:
    // Call the member function
    void exec(Event * evnt) {
        call(evnt);
    }
    ~IEventHandler() = default;

private:
    // Implemented by MemberFunctionHandler
    virtual void call(Event * evnt) = 0;
};

template<class T, class EventType>
class EventHandler : public IEventHandler
{
public:
    typedef void (T::*MemberFunction)(EventType*);
    EventHandler(T * i, MemberFunction mf) {
        instance = i;
        memberFunction = mf;
    }

    void call(Event * e) {
        // Cast event to the correct type and call member function
        (instance->*memberFunction)(static_cast<EventType*>(e));
    }

private:
    // Pointer to class instance
    T * instance;

    // Pointer to member function
    MemberFunction memberFunction;
};
#endif //CAPTURE_THE_CASTLE_EVENT_HANDLER_HPP
