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

	bool init(const vec2& screen_size);
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
	static const int MAX_PARTICLE = 300;
	static const int NUM_PARTICLE = 100;

	float air_density = 1.1455f;
	float air_speed = 1.f;
	const float DRAG_COEF_SPHERE = 0.47f;
	const float GRAVITY_ACC = 9.81f;
	const float PARTICLE_RADIUS = 4.f;
	const float PARTICLE_LIFE = 4.f;

	const float SPAWN_DELAY = 0.f;
	const float SPAWN_GROUP_DELAY = 10.f;

	const float ENEMY_MASS = 8.f;
	const float PLAYER_MASS = 5.f;

private:
	void setup_randomness(const vec2& screen_size);
	void destroy();

	void handle_motion(const float& dt);
	void handle_spawn(const float& dt);
	void handle_particle_life();
	void handle_collisions(const float& dt);
	bool is_okay_to_collide_with(const CollisionLayer& collision_layer);
	void handle_particle_particle_collision(Particle& p1, Particle& p2);
	void handle_particle_entity_collision(
		RainSystem::Particle& p1, Transform& e_transform, Motion& e_motion,
		const CollisionLayer& collision_layer, const float& dt
	);
	void do_hail(const vec2& position);

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
	std::uniform_real_distribution<float> m_dist_SpawnDelay;

	Mesh mesh{};
	Effect effect{};

	Mix_Chunk* m_pop;
};

#endif //CAPTURE_THE_CASTLE_RAIN_SYSTEM_HPP


