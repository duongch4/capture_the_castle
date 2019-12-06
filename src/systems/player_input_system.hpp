//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <ecs/events.hpp>
#include <components.hpp>
#include <texture_manager.hpp>
#include <tilemap.hpp>
#include <random>
#include <SDL_mixer.h>

extern ECSManager ecsManager;

class PlayerInputSystem : public System {
public:
    void init(std::shared_ptr<Tilemap> tilemap);
    void update();
    void reset() override;
	static const size_t get_max_soldiers() { return 2 * MAX_SOLDIERS; };

private:
    const std::vector<InputKeys> PLAYER1KEYS = {
		InputKeys::W, InputKeys::A, InputKeys::S, InputKeys::D,
		InputKeys::Q, InputKeys:: LEFT_SHIFT
	};

    const std::vector<InputKeys> PLAYER2KEYS = {
		InputKeys::UP, InputKeys::DOWN, InputKeys::RIGHT, InputKeys::LEFT,
		InputKeys::SLASH, InputKeys:: RIGHT_SHIFT
	};

	std::map<InputKeys, bool> keysPressed;

	std::shared_ptr<Tilemap> m_tilemap;

private:
    void onKeyListener(InputKeyEvent* input);
    void onReleaseListener(KeyReleaseEvent* input);
    void onTimeoutListener(TimeoutEvent* input);

private:
	size_t soldier_count_1 = 0;
	size_t soldier_count_2 = 0;
	static const size_t MAX_SOLDIERS = 2;

	size_t wait_1 = 0;
	size_t wait_2 = 0;
	static const size_t RESPAWN_IDLE = 30;

private:
	void handle_soldier_spawn(
		size_t& soldier_count, size_t& wait_time, const Transform& transform, const Tile& tile,
		const MazeRegion& maze_region, const TeamType& team_type, const char* texture_path
	);
	bool can_spawn(
		const size_t& soldier_count, const size_t& wait_time, const Transform& transform,
		const Tile& tile, const MazeRegion& maze_region
	);
	void spawn_soldier(
		const Transform& transform, const Motion& motion,
		const TeamType& team_type, const char* texture_path
	);

	void place_bomb(const Tile& tile, const TeamType& team_type);

private:
	// C++ rng
	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;
	std::random_device rd;
    Mix_Chunk* bomb_set_sound;
    Mix_Chunk* soldier_set_sound;
};


#endif //CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
