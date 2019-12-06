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
#include <tilemap.hpp>
#include <curve.hpp>
#include <mesh_manager.hpp>

extern ECSManager ecsManager;

class RainSystem : public System
{
public:
	bool init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players);
	void update(float& ms);
	void reset() override;

public:
	struct Particle
	{
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
	std::uniform_real_distribution<float> m_dist_Velocity;
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


