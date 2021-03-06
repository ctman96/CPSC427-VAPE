//
// Created by Cody on 11/20/2019.
//

#include <sstream>
#include "BetweenLevelsState.hpp"
#include "LevelState.hpp"
#include "MainMenuState.hpp"


BetweenLevelsState::BetweenLevelsState(const Levels::Level prev_level, unsigned int prev_start_points, const PlayerData &player_data) :
        m_prev_level(prev_level), m_prev_start_points(prev_start_points), m_player_data(player_data), m_font(Font(font_path("spaceranger.ttf"))) {
}

void BetweenLevelsState::init() {
    m_continue.init();

    int w, h;
    glfwGetFramebufferSize(GameEngine::getInstance().getM_window(), &w, &h);
    vec2 screen = { (float)w / GameEngine::getInstance().getM_screen_scale(), (float)h / GameEngine::getInstance().getM_screen_scale() };

    m_text.emplace_back();
    m_text.back().init(&m_font);
    std::stringstream ss;
    ss << "Level " << m_prev_level.id <<" Completed!" << std::endl;
    m_text.back().setText(ss.str().c_str());
    m_text.back().setColor({0.5f, 0.1f, 0.8f});
    m_text.back().setPosition({screen.x/2 - m_text.back().getBoundingBox().x/2, screen.y/2});
}

void BetweenLevelsState::terminate() {
    m_continue.destroy();
    for (auto& text : m_text) {
        text.destroy();
    }
}

void BetweenLevelsState::update(float ms) {

}

void BetweenLevelsState::draw() {
    // Clearing error buffer
    gl_flush_errors();

    // Getting size of window
    int w, h;
    GLFWwindow* m_window = GameEngine::getInstance().getM_window();
    glfwGetFramebufferSize(m_window, &w, &h);

    // Updating window title with points
    std::stringstream title_ss;
    title_ss << "V.A.P.E";
    glfwSetWindowTitle(m_window, title_ss.str().c_str());

    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
    const float clear_color[3] = { 0.f, 0.f, 0.f };
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w / GameEngine::getInstance().getM_screen_scale();// *0.5;
    float bottom = (float)h / GameEngine::getInstance().getM_screen_scale();// *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

    m_continue.draw(projection_2D);
    for (auto& text : m_text) {
        text.draw(projection_2D);
    }

    //////////////////
    // Presenting
    glfwSwapBuffers(m_window);
}

void BetweenLevelsState::on_key(GLFWwindow *wwindow, int key, int i, int action, int mod) {
    if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER)
    {
        if (m_prev_level.nextLevel) {
            GameEngine::getInstance().changeState(new LevelState(*m_prev_level.nextLevel, m_player_data));
            return;
        } else {
            // This shouldn't normally be reached, but added in case
            GameEngine::getInstance().changeState(new MainMenuState());
            return;
        }

    }
}

void BetweenLevelsState::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {

}

void BetweenLevelsState::on_mouse_button(GLFWwindow *window, int button, int action, int mods) {

}
