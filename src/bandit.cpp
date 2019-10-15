////
//// Created by Owner on 2019-09-29.
////
//
//#include <ctgmath>
//#include "bandit.hpp"
//#include "common.hpp"
//
//Texture Bandit::bandit_texture;
//
//Bandit::Bandit() {
//    // Load shared texture
//    if (!bandit_texture.is_valid()) {
//        if (!bandit_texture.load_from_file(textures_path("bandit/CaptureTheCastle_bandit_right.png"))) {
//            fprintf(stderr, "Failed to load solider texture!");
//        }
//    }
//    motion.direction = {1.f, 0.f};
//    motion.speed = 50.f;
//    physics.scale = {0.3f, 0.3f};
//    collision.bounding_box =
//            {
//                    std::fabs(physics.scale.x) * bandit_texture.width,
//                    std::fabs(physics.scale.y) * bandit_texture.height
//            };
//    team.assigned = Team::BANDIT;
//}
//
//bool Bandit::init() {
//
//    // The position corresponds to the center of the texture.
//    float wr = bandit_texture.width * 0.5f;
//    float hr = bandit_texture.height * 0.5f;
//
//    TexturedVertex vertices[4];
//    vertices[0].position = {-wr, +hr, -0.01f};
//    vertices[0].texcoord = {0.f, 1.f};
//    vertices[1].position = {+wr, +hr, -0.01f};
//    vertices[1].texcoord = {1.f, 1.f,};
//    vertices[2].position = {+wr, -hr, -0.01f};
//    vertices[2].texcoord = {1.f, 0.f};
//    vertices[3].position = {-wr, -hr, -0.01f};
//    vertices[3].texcoord = {0.f, 0.f};
//
//    // Counterclockwise as it's the default opengl front winding direction.
//    uint16_t indices[] = {0, 3, 1, 1, 3, 2};
//
//    // Clearing errors
//    gl_flush_errors();
//
//    // Vertex Buffer creation
//    glGenBuffers(1, &mesh.vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
//
//    // Index Buffer creation
//    glGenBuffers(1, &mesh.ibo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
//
//    // Vertex Array (Container for Vertex + Index buffer)
//    glGenVertexArrays(1, &mesh.vao);
//    if (gl_has_errors())
//        return false;
//
//    // Loading shaders
//    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
//        return false;
//
//    return true;
//}
//
//void Bandit::update(float ms) {
//    float step = motion.speed * (ms / 800);
//    position.pos_x += motion.direction.x * step;
//    position.pos_y += motion.direction.y * step;
//}
//
//void Bandit::draw(const mat3 &projection) {
//    // Transformation code, see Rendering and Transformation in the template specification for more info
//    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
//    transform.begin();
//    transform.translate({position.pos_x, position.pos_y});
//    transform.scale(physics.scale);
//    transform.end();
//
//    // Setting shaders
//    glUseProgram(effect.program);
//
//    // Enabling alpha channel for textures
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_DEPTH_TEST);
//
//    // Getting uniform locations for glUniform* calls
//    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
//    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
//    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
//
//    // Setting vertices and indices
//    glBindVertexArray(mesh.vao);
//    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
//
//    // Input data location as in the vertex buffer
//    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
//    GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
//    glEnableVertexAttribArray(in_position_loc);
//    glEnableVertexAttribArray(in_texcoord_loc);
//    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) 0);
//    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) sizeof(vec3));
//
//    // Enabling and binding texture to slot 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, bandit_texture.id);
//
//    // Setting uniform values to the currently bound program
//    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *) &transform.out);
//    float color[] = {1.f, 1.f, 1.f};
//    glUniform3fv(color_uloc, 1, color);
//    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *) &projection);
//
//    // Drawing!
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
//}
//
//// Releases all graphics resources
//void Bandit::destroy() {
//    glDeleteBuffers(1, &mesh.vbo);
//    glDeleteBuffers(1, &mesh.ibo);
//    glDeleteBuffers(1, &mesh.vao);
//
//    glDeleteShader(effect.vertex);
//    glDeleteShader(effect.fragment);
//    glDeleteShader(effect.program);
//}
//
//Bandit::~Bandit() {
//
//}
//
//void Bandit::set_position(vec2 pos) {
//    position.pos_x = pos.x;
//    position.pos_y = pos.y;
//}
//
//void Bandit::set_direction(vec2 dir) {
//    motion.direction = dir;
//}
//
//vec2 Bandit::get_bounding_box() const {
//    return collision.bounding_box;
//}
//
//vec2 Bandit::get_position() {
//    return {position.pos_x, position.pos_y};
//}
//
//vec2 Bandit::get_direction() {
//    return motion.direction;
//}
//
//bool Bandit:: collides_with_tile(const Tile& tile){
//    float bl = position.pos_x;
//    float br = bl + get_bounding_box().x;
//    float bt = position.pos_y;
//    float bb = bt + get_bounding_box().y;
//
//    float tt = tile.get_position().y;
//    float tb = tt + tile.get_bounding_box().y;
//    float tl = tile.get_position().x;
//    float tr = tl + tile.get_bounding_box().x;
//
//    bool x_overlap = (br >= tl && br <= tr) || (bl <= tr && bl >= tl) || (br <= tl && bl >= tl);
//    bool y_overlap = (bt >= tt && bt <= tb) || (bb <= tb && bb >= tt) || (bb <= tb && bt >= tt);
//
//    return x_overlap && y_overlap;
//}
//
//void Bandit::handle_wall_collision() {
//    if (motion.direction.x > 0){
//        position.pos_x -= 3.f;
//    } else{
//        position.pos_x += 3.f;
//    }
//   motion.direction = {-motion.direction.x, motion.direction.y};
//}
//
//
//
