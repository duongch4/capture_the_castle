//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_EVENT_MANAGER_HPP
#define CAPTURE_THE_CASTLE_EVENT_MANAGER_HPP

#include <functional>
#include <map>
#include <typeindex>
#include "common_ecs.hpp"
#include "event_handler.hpp"

typedef std::vector<IEventHandler*> HandlerList;
class EventManager {
public:
    template<typename EventType>
    void publish(EventType* e) {
        HandlerList * handlers = subscribers[typeid(EventType)];

        if (handlers == nullptr) {
            return;
        }

        for (auto & handler : *handlers) {
            if (handler != nullptr) {
                handler->exec(e);
            }
        }
    }

    template<class T, class EventType>
    void subscribe(T* instance, void (T::*memberFunction)(EventType*)) {
        HandlerList* handlers = subscribers[typeid(EventType)];

        //First time initialization
        if (handlers == nullptr) {
            handlers = new HandlerList();
            subscribers[typeid(EventType)] = handlers;
        }

        handlers->push_back(new EventHandler<T, EventType>(instance, memberFunction));
    }
private:
    std::map<std::type_index, HandlerList*> subscribers;
};
#endif //CAPTURE_THE_CASTLE_EVENT_MANAGER_HPP
