//
// Created by Cody on 11/20/2019.
//

#ifndef VAPE_BULLETSTRAIGHTSHOT_HPP
#define VAPE_BULLETSTRAIGHTSHOT_HPP


#include "Weapon.hpp"

class BulletStraightShot : public Weapon {
private:
    Mix_Chunk* m_bullet_sound;
    float m_bullet_cooldown;
public:
    void init() override;

    Projectile* fire(const vec2 &origin_position, float origin_rotation) override;

    void update(float ms) override;

    void destroy() override;
};


#endif //VAPE_BULLETSTRAIGHTSHOT_HPP