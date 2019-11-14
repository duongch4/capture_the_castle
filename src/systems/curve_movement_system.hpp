//
// Created by Owner on 2019-11-11.
//

#ifndef CAPTURE_THE_CASTLE_CURVE_MOVEMENT_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_CURVE_MOVEMENT_SYSTEM_HPP

#include <ecs/common_ecs.hpp>
#include <curve.hpp>
#include <ecs/ecs_manager.hpp>
#include <components.hpp>

extern ECSManager ecsManager;

class CurveMovementSystem : public System {
public:
    void init() {
        time = 0.f;
    };
    void update(float t) {
        for (auto &e: entities) {
            auto& motion = ecsManager.getComponent<CurveMotionComponent>(e);
            auto& transform = ecsManager.getComponent<Transform>(e);
            curve.set_control_points({motion.p0, motion.p1, motion.p2, motion.p3});
            vec2 pos_curve = curve.get_curve_points(time);
            transform.position.y = transform.init_position.y - pos_curve.y;
        }
        next_time();
    };
    void reset() override {

    };

private:
    Curve curve;
    float normalize_time(float t) {
        return fmodf(t, 1000.f) / 1000.f;
    }
    void next_time() {
        if (time + time_step <= 1.0f && time + time_step >= 0.f) {
            time += time_step;
        } else {
            time_step = -time_step;
            time += time_step;
        }
    }
    float time;
    float step = 0.01f;
    float time_step = step;
};
#endif //CAPTURE_THE_CASTLE_CURVE_MOVEMENT_SYSTEM_HPP
