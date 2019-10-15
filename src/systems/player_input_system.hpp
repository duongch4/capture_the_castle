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
        ecsManager.subscribe(this, &PlayerInputSystem::inputListener);
    }
    void update() {
        for (auto& e: entities) {
            auto& motion = ecsManager.getComponent<Motion>(e);
            auto& transform = ecsManager.getComponent<Transform>(e);
            auto& team = ecsManager.getComponent<Team>(e);

            if(team.assigned == TeamType::PLAYER2) {
                switch(key) {
                    case InputKeys::UP :
                        motion.direction = {0, -1};
                        break;
                    case InputKeys::DOWN :
                        motion.direction = {0, 1};
                        break;
                    case InputKeys::RIGHT :
                        motion.direction = {1, 0};
                        transform.scale = {-transform.scale.x, transform.scale.y};
                        break;
                    case InputKeys::LEFT :
                        transform.scale = {-transform.scale.x, transform.scale.y};
                        motion.direction = {-1, 0};
                        break;
                    default:
                        motion.direction = {0, 0};
                        break;
                }
            } else if (team.assigned == TeamType::PLAYER1) {
                switch(key) {
                    case InputKeys::W :
                        motion.direction = {0, -1};
                        break;
                    case InputKeys::S :
                        motion.direction = {0, 1};
                        break;
                    case InputKeys::D :
                        transform.scale = {-transform.scale.x, transform.scale.y};
                        motion.direction = {1, 0};
                        break;
                    case InputKeys::A :
                        transform.scale = {-transform.scale.x, transform.scale.y};
                        motion.direction = {-1, 0};
                        break;
                    default:
                        motion.direction = {0, 0};
                        break;
                }
            }
        }
    }

private:
    InputKeys key = InputKeys::DEFAULT;
    void inputListener(InputKeyEvent* input) {
        key = input->key;
    };
};


#endif //CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
