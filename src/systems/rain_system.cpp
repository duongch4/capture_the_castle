#define _USE_MATH_DEFINES
#include "rain_system.hpp"

constexpr int NUM_SEGMENTS = 8;

bool RainSystem::init(const vec2& screen_size) {
    m_rng = std::default_random_engine(std::random_device()());
	setup_randomness(screen_size);

    m_spawn_timer = SPAWN_DELAY;
    m_spawn_count = 0;

    std::vector<GLfloat> screen_vertex_buffer_data;
    constexpr float z = -0.01f;

    for (int i = 0; i < NUM_SEGMENTS; i++) {
        screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(z);

        screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(z);

        screen_vertex_buffer_data.push_back(0);
        screen_vertex_buffer_data.push_back(0);
        screen_vertex_buffer_data.push_back(z);
    }

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size()*sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

    if (gl_has_errors())
        return false;

    //m_pop = Mix_LoadWAV(audio_path("capturethecastle_firecracker.wav"));

    return effect.load_from_file(shader_path("firework.vs.glsl"), shader_path("firework.fs.glsl"));
}

void RainSystem::setup_randomness(const vec2 & screen_size)
{
	std::uniform_real_distribution<float> distVelocityX(50.f, 1000.f);
	std::uniform_real_distribution<float> distVelocityY(50.f, 500.f);
	std::uniform_real_distribution<float> distPositionX(0.f, screen_size.x);
	std::uniform_real_distribution<float> distPositionY(0.f, 0.f);
	std::uniform_real_distribution<float> distColor(0.7f, 1.f);
	std::uniform_real_distribution<float> distRadian(-M_PI, M_PI);
	std::uniform_real_distribution<float> distSpawnDelay(5.f, 10.f);
	m_dist_PositionX = distPositionX;
	m_dist_PositionY = distPositionY;
	m_dist_Color = distColor;
	m_dist_Radian = distRadian;
	m_dist_VelocityX = distVelocityX;
	m_dist_VelocityY = distVelocityY;
	m_dist_SpawnDelay = distSpawnDelay;
}

void RainSystem::destroy() {
	if (m_pop != nullptr)
		Mix_FreeChunk(m_pop);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &m_instance_vbo);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
	//m_curve.clear_points();
    m_particles.clear();
	m_particles.shrink_to_fit();
}

void RainSystem::update(const float& ms) {
    float seconds = ms / 1000;
	handle_spawn(seconds);
	handle_particle_life();
	handle_motion(seconds);
	handle_collisions(seconds);
}

void RainSystem::handle_collisions(const float& dt)
{
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		auto& p1 = m_particles[i];
		for (size_t j = i + 1; j < m_particles.size(); ++j)
		{
			auto& p2 = m_particles[j];
			if (intersect_circle_circle(p1.position, p2.position, p1.radius, p2.radius))
			{
				handle_particle_particle_collision(p1, p2);
			}
		}
		for (auto& entity : entities)
		{
			const C_Collision collision_comp = ecsManager.getComponent<C_Collision>(entity);
			const CollisionLayer collision_layer = collision_comp.layer;
			if (is_okay_to_collide_with(collision_layer))
			{
				Transform& e_transform = ecsManager.getComponent<Transform>(entity);
				if (intersect_circle_circle(p1.position, e_transform.position, p1.radius, collision_comp.radius))
				{
					Motion& e_motion = ecsManager.getComponent<Motion>(entity);
					handle_particle_entity_collision(p1, e_transform, e_motion, collision_layer, dt);
				}
			}
		}
	}
}

void RainSystem::handle_particle_entity_collision(
	RainSystem::Particle& p1, Transform& e_transform, Motion& e_motion,
	const CollisionLayer& collision_layer, const float& dt
)
{
	float e_vel_x = e_motion.speed * e_motion.direction.x;
	float e_vel_y = e_motion.speed * e_motion.direction.y;

	float pos_x = e_transform.position.x - p1.position.x;
	float pos_y = e_transform.position.y - p1.position.y;

	float vel_x = p1.velocity.x - e_vel_x;
	float vel_y = p1.velocity.y - e_vel_y;
	float dotP = pos_x * vel_x + pos_y * vel_y;
	if (dotP > 0.f)
	{
		float e_mass = (collision_layer == CollisionLayer::Enemy) ? ENEMY_MASS : PLAYER_MASS;

		float mass_ratio_1 = 2 * e_mass / (p1.mass + e_mass);
		float vect_ratio_1 = dot(vec2{ vel_x,vel_y }, vec2{ -pos_x,-pos_y }) / sq_len(vec2{ -pos_x,-pos_y });
		p1.velocity = sub(p1.velocity, mul(vec2{ -pos_x,-pos_y }, mass_ratio_1 * vect_ratio_1));

		float mass_ratio_2 = 2 * p1.mass / (p1.mass + e_mass);
		float vect_ratio_2 = dot(vec2{ -vel_x,-vel_y }, vec2{ pos_x,pos_y }) / sq_len(vec2{ pos_x,pos_y });
		vec2 e_velocity = sub(vec2{ e_vel_x,e_vel_y }, mul(vec2{ pos_x,pos_y }, mass_ratio_2 * vect_ratio_2));

		e_transform.position = add(e_transform.position, mul(e_velocity, dt));
	}
}

bool RainSystem::is_okay_to_collide_with(const CollisionLayer& collision_layer)
{
	return collision_layer == CollisionLayer::Enemy || collision_layer == CollisionLayer::PLAYER1 || collision_layer == CollisionLayer::PLAYER2;
}

void RainSystem::handle_particle_particle_collision(RainSystem::Particle& p1, RainSystem::Particle& p2)
{
	float pos_x = p2.position.x - p1.position.x;
	float pos_y = p2.position.y - p1.position.y;
	float vel_x = p1.velocity.x - p2.velocity.x;
	float vel_y = p1.velocity.y - p2.velocity.y;
	float dotP = pos_x * vel_x + pos_y * vel_y;
	if (dotP > 0.f)
	{
		float mass_ratio_1 = 2 * p2.mass / (p1.mass + p2.mass);
		float vect_ratio_1 = dot(vec2{ vel_x,vel_y }, vec2{ -pos_x,-pos_y }) / sq_len(vec2{ -pos_x,-pos_y });
		p1.velocity = sub(p1.velocity, mul(vec2{ -pos_x,-pos_y }, mass_ratio_1 * vect_ratio_1));

		float mass_ratio_2 = 2 * p1.mass / (p1.mass + p2.mass);
		float vect_ratio_2 = dot(vec2{ -vel_x,-vel_y }, vec2{ pos_x,pos_y }) / sq_len(vec2{ pos_x,pos_y });
		p2.velocity = sub(p2.velocity, mul(vec2{ pos_x,pos_y }, mass_ratio_2 * vect_ratio_2));
	}
}

void RainSystem::handle_motion(const float& dt)
{
	// Update each particle's life, velocity, position
	for (auto& particle : m_particles)
	{
		particle.life -= dt;

		// F = 0.5 * density * speed^2 * drag_coef * surface_area
		float air_drag_force = 0.5f * air_density * air_speed * air_speed * DRAG_COEF_SPHERE * 4.f * 3.1415f * particle.radius * particle.radius;
		vec2 force = { -air_drag_force, particle.mass * GRAVITY_ACC };
		vec2 acc = { force.x / particle.mass, force.y / particle.mass };
		particle.velocity.x += acc.x * dt;
		particle.velocity.y += acc.y * dt;
		particle.position.x += particle.velocity.x * dt;
		particle.position.y += particle.velocity.y * dt;
	}
}

void RainSystem::handle_spawn(const float& dt)
{
	m_spawn_timer -= dt;

	if (m_spawn_timer < 0.f)
	{
		do_hail(vec2{ m_dist_PositionX(m_rng), m_dist_PositionY(m_rng) });
		m_spawn_count++;
		if (m_spawn_count > 100)
		{
			m_spawn_count = 0;
			m_spawn_timer = m_dist_SpawnDelay(m_rng);
		}
		else
		{
			m_spawn_timer = SPAWN_DELAY;
		}
	}
}

void RainSystem::handle_particle_life()
{
	// Remove the particle if life is smaller than zero
	auto particle_it = m_particles.begin();
	while (particle_it != m_particles.end())
	{
		if (particle_it->life <= 0)
		{
			particle_it = m_particles.erase(particle_it);
			continue;
		}
		++particle_it;
	}
}

void RainSystem::do_hail(const vec2& position) {
    if (m_particles.size() < MAX_PARTICLE) {
        for (int i = 0; i < NUM_PARTICLE; i++) {
            Particle particle;

            float randomRadian = m_dist_Radian(m_rng);
			float randomVelocityX = m_dist_VelocityX(m_rng);
			float randomVelocityY = m_dist_VelocityY(m_rng);

            particle.position = position;
            particle.velocity.x = randomVelocityX * cos(randomRadian);
            particle.velocity.y = randomVelocityY * sin(randomRadian);
            particle.radius = PARTICLE_RADIUS;
            particle.life = PARTICLE_LIFE;
			particle.color = vec3{ m_dist_Color(m_rng), m_dist_Color(m_rng), m_dist_Color(m_rng) };

            m_particles.emplace_back(particle);
        }
    }
    //Mix_PlayChannel(-1, m_pop, 0);
}

void RainSystem::draw(const mat3& projection) {
    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Getting uniform locations
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Draw the screen texture on the geometry
    // Setting vertices
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    // Mesh vertex positions
    // Bind to attribute 0 (in_position) as in the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(0, 0);

    // Load up particles into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(Particle), m_particles.data(), GL_DYNAMIC_DRAW);

    // Particle translations
    // Bind to attribute 1 (in_translate) as in vertex shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, position));
    glVertexAttribDivisor(1, 1);

    // Particle radii
    // Bind to attribute 2 (in_scale) as in vertex shader
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, radius));
    glVertexAttribDivisor(2, 1);

    // Particle color
    // Bind to attribute 3 (in_color) as in vertex shader
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color));
    glVertexAttribDivisor(3, 1);

    // Draw using instancing
    glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS*3, m_particles.size());

    // Reset divisor
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glVertexAttribDivisor(3, 0);
}

void RainSystem::reset()
{
	this->destroy();
	this->entities.clear();
}