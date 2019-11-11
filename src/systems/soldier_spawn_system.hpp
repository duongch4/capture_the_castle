//#ifndef CAPTURE_THE_CASTLE_SOLDIER_SPAWN_SYSTEM_HPP
//#define CAPTURE_THE_CASTLE_SOLDIER_SPAWN_SYSTEM_HPP
//
//#include <memory>
//#include <random>
//
//#include <systems/player_input_system.hpp>
//#include <ecs/common_ecs.hpp>
//#include <tilemap.hpp>
//#include <texture_manager.hpp>
//
//
//class SoldierSpawnSystem: public System {
//public:
//    bool init(std::shared_ptr<Tilemap> tilemap);
//    void update(float ms);
//	static const size_t get_max_soldiers() { return MAX_SOLDIERS; }
//
//private:
//	static const size_t MAX_SOLDIERS = 3;
//    const size_t SOLDIER_DELAY_MS = 10000;
//    float next_soldier_spawn;
//    std::shared_ptr<Tilemap> tilemap;
//
//    // C++ rng
//    std::default_random_engine rng;
//    std::uniform_real_distribution<float> dist;
//
//	void spawn_soldier();
//};
//
//
//#endif //CAPTURE_THE_CASTLE_SOLDIER_SPAWN_SYSTEM_HPP
