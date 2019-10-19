//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <ecs/events.hpp>

extern ECSManager ecsManager;

class PlayerInputSystem : public System {
public:
    void init();
    void update();

private:
    const std::vector<InputKeys> PLAYER1KEYS = {InputKeys::W, InputKeys::A, InputKeys::S, InputKeys::D};
    const std::vector<InputKeys> PLAYER2KEYS = {InputKeys::UP, InputKeys::DOWN, InputKeys::RIGHT, InputKeys::LEFT};
    std::map<InputKeys, bool> keysPressed;
    void onKeyListener(InputKeyEvent* input);
    void onReleaseListener(KeyReleaseEvent* input);
};


#endif //CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
