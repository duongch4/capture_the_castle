#ifndef CAPTURE_THE_CASTLE_RAIN_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_RAIN_SYSTEM_HPP

#include <memory>
#include <random>
#include <cmath>

#include <iostream>

#include <SDL_mixer.h>

#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <common.hpp>
#include <tilemap.hpp>
#include <curve.hpp>
#include <mesh_manager.hpp>

extern ECSManager ecsManager;

class RainSystem : public System
{
public:
	void reset() override;

	bool init(vec2 screen_size);
	void destroy();
	void update(const float& ms);
	void draw(const mat3& projection);

private:
	struct Particle
	{
		float life = 0.f;
		vec2 position = { 0.f, 0.f };
		vec2 velocity = { 0.f, 0.f };
		float radius = 1.f;
		vec3 color = { 0.f, 0.f, 0.f };
		float mass = 1.f;
	};
	static const int MAX_PARTICLE = 500;
	static const int NUM_PARTICLE = 100;

	float air_density = 1.1455f;
	float air_speed = 1.f;
	const float DRAG_COEF_SPHERE = 0.47f;
	const float GRAVITY_ACC = 9.81f;
	const float PARTICLE_RADIUS = 4.f;
	const float PARTICLE_LIFE = 4.f;

	const float SPAWN_DELAY = 0.f;
	//static const float SPAWN_DELAY = 0.1f;
	const float SPAWN_GROUP_DELAY = 0.f;
	//static const float SPAWN_GROUP_DELAY = 0.6f;

private:
	void handle_motion(float seconds);
	void handle_spawn(float seconds);
	void handle_particle_life();
	void handle_collisions();
	void handle_particle_particle_collision(Particle& p1, Particle& p2);
	void kaboom(vec2 position);


	void next_time();
	GLuint m_instance_vbo;
	std::vector<Particle> m_particles;

	float m_spawn_timer;
	int m_spawn_count;
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist_PositionX;
	std::uniform_real_distribution<float> m_dist_PositionY;
	std::uniform_real_distribution<float> m_dist_Color;
	std::uniform_real_distribution<float> m_dist_Radian;
	std::uniform_real_distribution<float> m_dist_VelocityX;
	std::uniform_real_distribution<float> m_dist_VelocityY;
	std::uniform_real_distribution<float> m_dist_SpawnTimer;

	Curve m_curve;

	Mesh mesh{};
	Effect effect{};

	float time = 0.f;
	float step = 0.1f;
	float time_step = step;

	Mix_Chunk* m_pop;
};

#endif //CAPTURE_THE_CASTLE_RAIN_SYSTEM_HPP


