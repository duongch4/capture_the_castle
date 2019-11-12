//
// Created by Owner on 2019-10-12.
//


#include "player_input_system.hpp"

extern ECSManager ecsManager;

void PlayerInputSystem::init() {
    ecsManager.subscribe(this, &PlayerInputSystem::onKeyListener);
    ecsManager.subscribe(this, &PlayerInputSystem::onReleaseListener);
}

void PlayerInputSystem::update() {
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
                            next_dir.y -= 1;
                            break;
                        case InputKeys::S :
							next_dir.y += 1;
                            break;
                        case InputKeys::D :
							next_dir.x += 1;
                            break;
                        case InputKeys::A :
							next_dir.x -= 1;
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
							next_dir.y -= 1;
                            break;
                        case InputKeys::DOWN :
							next_dir.y += 1;
                            break;
                        case InputKeys::RIGHT :
							next_dir.x += 1;
                            break;
                        case InputKeys::LEFT :
							next_dir.x -= 1;
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

void PlayerInputSystem::onKeyListener(InputKeyEvent* input) {
    keysPressed[input->key] = true;
}

void PlayerInputSystem::onReleaseListener(KeyReleaseEvent* input) {
    keysPressed[input->keyReleased] = false;
}

void PlayerInputSystem::reset() {
    keysPressed.clear();
}
