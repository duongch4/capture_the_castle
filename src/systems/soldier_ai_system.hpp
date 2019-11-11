#ifndef CAPTURE_THE_CASTLE_SOLDIER_AI_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_SOLDIER_AI_SYSTEM_HPP

#include <memory>
#include <random>

#include <iostream>

#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <tilemap.hpp>
#include <systems/player_input_system.hpp>

extern ECSManager ecsManager;

class SoldierAiSystem : public System
{
public:
	bool init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players);
	void update(float& ms);

private:
	enum struct State
	{
		IDLE,
		PATROL
	};

	const size_t MAX_SOLDIERS = PlayerInputSystem::get_max_soldiers();
	const size_t IDLE_LIMIT = 10; // beats
	const size_t PATROL_LIMIT = 100;
	const float BASE_SPEED = 40.f;

	std::vector<size_t> m_idle_times;
	std::vector<size_t> m_patrol_times;
	std::vector<State> m_states;
	std::vector<Entity> m_soldiers;
	std::vector<Entity> m_targets;
	std::shared_ptr<Tilemap> m_tilemap;

	std::vector<vec2> m_prev_dirs;

private:
	void handle_idle(State& state, size_t& idle_time, const Entity& soldier);
	void handle_patrol(State& state, size_t& patrol_time, const Entity& soldier, vec2& prev_dir);

	bool can_move(const Entity& soldier, const Tile& tile);
	bool is_within_soldier_region(const Entity& soldier, const Tile& tile);

private:
	// C++ rng
	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;
	std::random_device rd;

private:
	void follow_direction(Entity& target, Entity& soldier, float& speed, float& elapsed_ms);
	bool is_target_move_toward_soldier(vec2& soldier_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);
	bool is_target_move_away_soldier(vec2& soldier_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);
};


#endif //CAPTURE_THE_CASTLE_SOLDIER_AI_SYSTEM_HPP
