//
// Created by Knox on 11/11/2019.
//
#define _USE_MATH_DEFINES
#include "firework.hpp"

static const int MAX_PARTICLE = 2000;
static const int NUM_PARTICLE = 100;
constexpr int NUM_SEGMENTS = 8;
static const float GRAVITY = 500.f;
static const int PARTICLE_SIZE = 4;
static const int PARTICLE_LIFE = 4;
static const float SPAWN_DELAY = 0.1f;
static const float SPAWN_GROUP_DELAY = 0.6f;

bool Firework::init(vec2 screen_size) {
    m_rng = std::default_random_engine(std::random_device()());
    std::uniform_real_distribution<float> distVelocity(50, 300);
    std::uniform_real_distribution<float> distPositionX(0 + screen_size.x / 5, screen_size.x - screen_size.x / 5);
    std::uniform_real_distribution<float> distPositionY(0 + screen_size.y / 5, 0 + screen_size.y / 2);
    std::uniform_real_distribution<float> distColor(0, 1);
    std::uniform_real_distribution<float> distRadian(-M_PI, M_PI);
    std::uniform_real_distribution<float> distSpawnTimer(0.8, 2.5);
    m_dist_PositionX = distPositionX;
    m_dist_PositionY = distPositionY;
    m_dist_Color = distColor;
    m_dist_Radian = distRadian;
    m_dist_Velocity = distVelocity;
    m_dist_SpawnTimer = distSpawnTimer;

    m_spawn_timer = SPAWN_DELAY;
    m_spawn_count = 0;

    m_curve.set_control_points({
        vec2{300.f, 350.f}, vec2{420.f, 30.f}, vec2{1000.f, 30.f}, vec2{1200.f, 350.f}
    });

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

    m_pop = Mix_LoadWAV(audio_path("capturethecastle_firecracker.wav"));

    return effect.load_from_file(shader_path("firework.vs.glsl"), shader_path("firework.fs.glsl"));
}

void Firework::destroy() {
	if (m_pop != nullptr)
		Mix_FreeChunk(m_pop);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &m_instance_vbo);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
	m_curve.clear_points();
    m_particles.clear();
	m_particles.shrink_to_fit();
}

void Firework::update(float ms) {
    float seconds = ms / 1000;
    m_spawn_timer -= seconds;

    if (m_spawn_timer <= 0) {
        // using the points on curve for position
        kaboom(m_curve.get_curve_points(time));
        next_time();
        m_spawn_count++;
        if (m_spawn_count == 3) {
            m_spawn_count = 0;
            m_spawn_timer = SPAWN_GROUP_DELAY;
        } else {
            m_spawn_timer = SPAWN_DELAY;
        }
    }

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

    // Update each particle's life, velocity, position
    for (auto& particle : m_particles) {
        particle.life -= seconds;
        particle.velocity.y += (GRAVITY * seconds);
        particle.position = vec2 {particle.position.x + (particle.velocity.x * seconds),
                                  particle.position.y + (particle.velocity.y * seconds)};
    }
}

void Firework::kaboom(vec2 position) {
    if (m_particles.size() < MAX_PARTICLE) {
        for (int i = 0; i < NUM_PARTICLE; i++) {
            Particle particle;

            float randomRadian = m_dist_Radian(m_rng);
            float randomVelocity = m_dist_Velocity(m_rng);

            particle.position = position;
            particle.velocity.x = randomVelocity * cos(randomRadian);
            particle.velocity.y = randomVelocity * sin(randomRadian);
            particle.radius = PARTICLE_SIZE;
            particle.life = PARTICLE_LIFE;
            particle.color = vec3{m_dist_Color(m_rng), m_dist_Color(m_rng), m_dist_Color(m_rng)};

            m_particles.emplace_back(particle);
        }
    }
    Mix_PlayChannel(-1, m_pop, 0);
}

void Firework::draw(const mat3& projection) {
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

void Firework::next_time() {
    if (time + time_step <= 1.0f && time + time_step >= 0.f) {
        time += time_step;
    } else {
        time_step = -time_step;
        time += time_step;
    }
}