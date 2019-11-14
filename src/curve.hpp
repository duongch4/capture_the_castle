//
// Created by Owner on 2019-11-11.
//

#ifndef CAPTURE_THE_CASTLE_CURVE_HPP
#define CAPTURE_THE_CASTLE_CURVE_HPP

#include <vector>
#include "common.hpp"

class Curve {
public:
    vec2 get_curve_points(float t) {
        float u = 1.f - t;
        float t2 = t * t;
        float u2 = u * u;
        float u3 = u2 * u;
        float t3 = t2 * t;

        vec2 p0 = m_points[0];
        vec2 p1 = m_points[1];
        vec2 p2 = m_points[2];
        vec2 p3 = m_points[3];

        vec2 a = mul(p0, u3);
        vec2 b = mul(p1, (3.f * t * u2));
        vec2 c = mul(p2, (3.f * t2 * u));
        vec2 d = mul(p3, t3);
        vec2 result = add(add(add(a, b), c), d);
        return result;
    }

    void set_control_points(std::vector<vec2> points) {
        m_points = points;
    }

private:
    std::vector<vec2> m_points;
};

#endif //CAPTURE_THE_CASTLE_CURVE_HPP
