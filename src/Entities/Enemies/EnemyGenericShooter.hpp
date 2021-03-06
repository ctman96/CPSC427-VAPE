//
// Created by Tanha Kabir on 2019-11-11.
//

#ifndef VAPE_ENEMYGENERICSHOOTER_HPP
#define VAPE_ENEMYGENERICSHOOTER_HPP

#include <Entities/Enemies/Enemy.hpp>
#include "common.hpp"

// Player enemy
class EnemyGenericShooter : public Enemy{
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

private:
    float m_bullet_cooldown;
    unsigned int m_burst_count;
    float m_burst_cooldown;

    void spawnBullet();
};

#endif //VAPE_ENEMYGENERICSHOOTER_HPP
