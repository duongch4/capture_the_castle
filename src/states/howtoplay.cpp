
#include "howtoplay.hpp"
#include "game.hpp"
#include "menu.hpp"

bool HowToPlay::init_state(World *world) {
    m_world = world;
    currInstructions = GOALS;
    background.init(m_world->get_screen_size());
    vec2 offset = {m_world->get_screen_size().x / 6, m_world->get_screen_size().y / 6};
    vec2 background_pos = background.get_position();
    instructions.init({background_pos.x - offset.x, background_pos.y}, textures_path("ui/CaptureTheCastle_how_to_play_instructions.png"));
    main_menu_btn.init({(float)(background_pos.x + offset.x * 1.5), (float) (background_pos.y - offset.y)}, textures_path("ui/CaptureTheCastle_main_menu_btn.png"));
    main_menu_btn.setScale({0.8, 0.4});
    new_game_btn.init({(float)(background_pos.x + offset.x * 1.5), (float) (background_pos.y)}, textures_path("ui/CaptureTheCastle_new_game_btn.png"));
    new_game_btn.setScale({0.8, 0.4});
    quit_btn.init({(float)(background_pos.x + offset.x * 1.5), (float) (background_pos.y + offset.y)}, textures_path("ui/CaptureTheCastle_quit_btn.png"));
    quit_btn.setScale({0.8, 0.4});
    controls_btn.init({(float)(background_pos.x + offset.x * 1.5), (float) (background_pos.y + 2 * offset.y)}, textures_path("ui/CaptureTheCastle_how_to_play_btn.png"));
    controls_btn.setScale({0.8, 0.4});

    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
    if (m_click == nullptr)
    {
        fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
                audio_path("capturethecastle_button_click.wav"));
        return false;
    }
    m_background_music = Mix_LoadMUS(audio_path("capturethecastle_main_menu.wav"));

    if (m_background_music == nullptr)
    {
        fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
                audio_path("capturethecastle_main_menu.wav"));
        return false;
    }
    Mix_PlayMusic(m_background_music, -1);
    return true;
}

void HowToPlay::on_key(int key, int action) {
    // no actions
}

void HowToPlay::on_mouse_click(GLFWwindow *window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        switch (checkButtonClicks({ (float) xpos, (float) ypos })) {
            case ButtonActions::RESTART:
                m_world->set_state(std::make_unique<Game>());
                break;
            case ButtonActions::QUIT:
                m_world->set_window_closed();
                break;
            case ButtonActions::MAIN:
                m_world->set_state(std::make_unique<Menu>());
                break;
            case ButtonActions::NONE:
                break;
        }
    }
}

void HowToPlay::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    new_game_btn.onHover(new_game_btn.mouseOnButton({(float) xpos, (float) ypos}));
    quit_btn.onHover(quit_btn.mouseOnButton({(float) xpos, (float) ypos}));
    main_menu_btn.onHover(main_menu_btn.mouseOnButton({(float) xpos, (float) ypos}));
    controls_btn.onHover(controls_btn.mouseOnButton({(float) xpos, (float) ypos}));
}

bool HowToPlay::update(float elapsed_ms) {
    return false;
}

void HowToPlay::draw() {
    background.draw(m_world->get_projection_2d());
    instructions.draw(m_world->get_projection_2d());
    quit_btn.draw(m_world->get_projection_2d());
    new_game_btn.draw(m_world->get_projection_2d());
    main_menu_btn.draw(m_world->get_projection_2d());
    controls_btn.draw(m_world->get_projection_2d());
}

void HowToPlay::reset() {
    // not needed
}

void HowToPlay::destroy() {
	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_click != nullptr)
	    Mix_FreeChunk(m_click);
    background.destroy();
    instructions.destroy();
    quit_btn.destroy();
    new_game_btn.destroy();
    main_menu_btn.destroy();
    controls_btn.destroy();
}

ButtonActions HowToPlay::checkButtonClicks(vec2 mouseloc) {
    if (new_game_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::RESTART;
    } else if (quit_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::QUIT;
    } else if (main_menu_btn.mouseOnButton(mouseloc)){
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::MAIN;
    } else if (controls_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        if (currInstructions == Page::CONTROLS) {
            currInstructions = Page::GOALS;
            instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_how_to_play_instructions.png"));
        } else if (currInstructions == Page::GOALS) {
            currInstructions = Page::CONTROLS;
            instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
        }
        return ButtonActions::NONE;
    } else {
        return ButtonActions::NONE;
    }
}
