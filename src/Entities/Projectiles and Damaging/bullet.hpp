//
// Created by Cody on 9/2/2019.
//

#ifndef VAPE_BULLET_HPP
#define VAPE_BULLET_HPP

#include "Projectile.hpp"

class Bullet : public Projectile {
    // Shared between all bullets, no need to load one for each instance
    static Texture bullet_texture;

public:
    bool init(vec2 position, float rotation, bool hostile, int damage) override;
    void destroy() override;
    void update(float ms) override;
    void draw(const mat3& projection) override;

    // Returns the current bullet position
    vec2 get_position()const override;

    // Collision routines for player, turtles and fish
    bool collides_with(const Player& player) override;
    bool collides_with(const Enemy& turtle) override;
	bool collides_with(const Boss& boss) override;//TODO either generic collides, or generic boss/enemy
	bool collides_with(const Clone& clone) override;

    // Returns the bullet' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box()const override;

    bool isOffScreen(const vec2 &screen) override;

    void set_speed_slow();
    void set_speed_fast();

private:
    float m_speed;
};

#endif //VAPE_BULLET_HPP
