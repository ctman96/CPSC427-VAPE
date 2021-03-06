//
// Created by Tanha Kabir on 2019-12-02.
//

#ifndef VAPE_ENEMYEXPLOSIVEPAYLOAD_HPP
#define VAPE_ENEMYEXPLOSIVEPAYLOAD_HPP

#include <Entities/Enemies/Enemy.hpp>
#include "common.hpp"

class EnemyExplosivePlayload: public Enemy {
    // Shared between all turtles, no need to load one for each instance
    static Texture texture;
public:
// Creates all the associated render resources and default transform
    bool init() override;

    // Releases all the associated resources
    void destroy() override;

    // Update turtle due to current
    // ms represents the number of milliseconds elapsed from the previous update() call
    void update(float ms) override;

    // Renders the salmon
    // projection is the 2D orthographic projection matrix
    void draw(const mat3& projection) override;

    // Returns the current turtle position
    vec2 get_position()const override;

    // Sets the new turtle position
    void set_position(vec2 position) override;

    // Returns the turtle' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box() const override;

    void set_velocity(vec2 velocity) override;

    void set_path(std::vector<vec2> path) override;
    std::vector<vec2> get_path() override;

private:
    float m_explosive_cooldown_ms;
    float m_rotate_cooldown_ms;
    float m_rotation_direction;
    std::vector<vec2> m_path;

    void spawnPayloadBullets();
};

#endif //VAPE_ENEMYEXPLOSIVEPAYLOAD_HPP
