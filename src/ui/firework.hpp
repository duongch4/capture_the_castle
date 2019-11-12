//
// Created by Knox on 11/11/2019.
//
#include <cmath>
#include <random>
#include <components.hpp>
#include <mesh_manager.hpp>

#ifndef CAPTURE_THE_CASTLE_FIREWORK_HPP
#define CAPTURE_THE_CASTLE_FIREWORK_HPP

class Firework
{
public:
    struct Particle {
        float life = 0.0f;
        vec2 position;
        vec2 velocity;
        float radius;
        vec3 color;
    };
    bool init(vec2 screen_size);

    void destroy();

    void update(float ms);

    void draw(const mat3& projection);

    void kaboom(vec2 position);
private:
    GLuint m_instance_vbo;
    std::vector<Particle> m_particles;

    float m_spawn_timer;
    std::default_random_engine m_rng;
    std::uniform_real_distribution<float> m_dist_PositionX;
    std::uniform_real_distribution<float> m_dist_PositionY;
    std::uniform_real_distribution<float> m_dist_Color;
    std::uniform_real_distribution<float> m_dist_Radian;
    std::uniform_real_distribution<float> m_dist_Velocity;
    std::uniform_real_distribution<float> m_dist_SpawnTimer;

    Mesh mesh{};
    Effect effect{};
};

#endif //CAPTURE_THE_CASTLE_FIREWORK_HPP


