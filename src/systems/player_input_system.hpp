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
    void init() {
        ecsManager.subscribe(this, &PlayerInputSystem::onKeyListener);
        ecsManager.subscribe(this, &PlayerInputSystem::onReleaseListener);
    }
    void update() {
        for (auto& e: entities) {
            auto& motion = ecsManager.getComponent<Motion>(e);
            auto& transform = ecsManager.getComponent<Transform>(e);
            auto& team = ecsManager.getComponent<Team>(e);

            vec2 next_dir = {0, 0};
            if (team.assigned == TeamType::PLAYER1) {
                for(auto key: PLAYER1KEYS) {
                    if (keysPressed[key]) {
                        switch(key) {
                            case InputKeys::W :
                                next_dir = {0, -1};
                                break;
                            case InputKeys::S :
                                next_dir = {0, 1};
                                break;
                            case InputKeys::D :
                                next_dir = {1, 0};
                                break;
                            case InputKeys::A :
                                next_dir = {-1, 0};
                                break;
                            default:
                                break;
                        }
                    }
                }
            } else if (team.assigned == TeamType::PLAYER2) {
                for (auto key: PLAYER2KEYS) {
                    if (keysPressed[key]) {
                        switch(key) {
                            case InputKeys::UP :
                                next_dir = {0, -1};
                                break;
                            case InputKeys::DOWN :
                                next_dir = {0, 1};
                                break;
                            case InputKeys::RIGHT :
                                next_dir = {1, 0};
                                break;
                            case InputKeys::LEFT :
                                next_dir = {-1, 0};
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            motion.direction = next_dir;
        }
    }

private:
    const std::vector<InputKeys> PLAYER1KEYS = {InputKeys::W, InputKeys::A, InputKeys::S, InputKeys::D};
    const std::vector<InputKeys> PLAYER2KEYS = {InputKeys::UP, InputKeys::DOWN, InputKeys::RIGHT, InputKeys::LEFT};
    std::map<InputKeys, bool> keysPressed;
    void onKeyListener(InputKeyEvent* input) {
        keysPressed[input->key] = true;
    };
    void onReleaseListener(KeyReleaseEvent* input) {
        keysPressed[input->keyReleased] = false;
    };
};


#endif //CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
