//! The state machine which handles running and swapping game states
/*!
 * The GameEngine class is a basic state machine designed to run
 * the different GameStates that make up the game.
 *
 * @author: Cody Newman
 *
 * Created on: 9/16/2019
 */

#ifndef VAPE_GAMEENGINE_HPP
#define VAPE_GAMEENGINE_HPP

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include <common.hpp>
#include <map>
#include "Engine/ECS/ECS.hpp"
#include "ECS/ECS.hpp"

class GameState;

class GameEngine {
public:
    // Singleton
    static GameEngine& getInstance() {
        static GameEngine INSTANCE;
        return INSTANCE;
    }
    GameEngine(GameEngine const&) = delete;
    void operator=(GameEngine const&) = delete;
    //

    //! Initializes the GameEngine
    void init();

    //! Cleans up the GameEngine in preparation for deletion
    void terminate();

    //! Switches to a new state
    /*!
     * @param state pointer to the state to be switched to
     */
    void changeState(GameState *state);

    //! Updates the state
    void update(float ms);

    //! Renders the state
    void draw();

    //! Returns whether the engine is currently running
    bool isRunning(){return running;}
    //! Sets the engine to stop running, for breaking out of loop
    void quit() {running = false;}

    //! INPUT CALLBACK FUNCTIONS
    void on_key(GLFWwindow*, int key, int, int action, int mod);
    void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
    void on_mouse_button (GLFWwindow* window, int button, int action, int mods);

    // Getters
    GLFWwindow *getM_window() const;

    float getM_screen_scale() const;

    float getElapsed_ms() const;

    float getM_current_speed() const;

    void setM_current_speed(float m_current_speed);

    void toggleM_debug_mode();

    bool getM_debug_mode();

    ECS::EntityManager *getEntityManager();
    ECS::SystemManager *getSystemManager();

private:
    GameEngine() = default; // private constructor

    //! Window handle
    GLFWwindow* m_window{};

    //! Screen to pixel coordinates scale factor
    float m_screen_scale{};

    //! The current loaded audio track
    Mix_Music * music{};

    ECS::EntityManager entityManager;
    ECS::SystemManager systemManager;

    float m_current_speed = 1.f;
    bool m_debug_mode = false;

    float elapsed_ms{};

    bool running{};
    GameState *state{};

    bool changingState = false;
};


#endif //VAPE_GAMEENGINE_HPP
