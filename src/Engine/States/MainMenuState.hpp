//! Game state representing the main menu
/*!
 * The MainMenu represents the game state
 * for the opening splash screen
 *
 * @author: Cody Newman
 *
 * Created on: 9/17/2019
 */

#ifndef VAPE_MAINMENUSTATE_HPP
#define VAPE_MAINMENUSTATE_HPP


#include <Engine/GameState.hpp>

class MainMenuState : public GameState{
    static Texture bg_texture;
public:
    void init() override;

    void terminate() override;

    void update(float ms) override;

    void draw() override;

    void on_key(GLFWwindow *wwindow, int key, int i, int action, int mod) override;

    void on_mouse_move(GLFWwindow *window, double xpos, double ypos) override;

    void on_mouse_button(GLFWwindow *window, int button, int action, int mods) override;

private:
	// Mouse position
	vec2 mouse_position;

};


#endif //VAPE_MAINMENUSTATE_HPP
