//
// Created by cody on 9/26/19.
//

#include "Boss1.hpp"
#include <cmath>
#include <iostream>
#include <Components/SpriteComponent.hpp>
#include <Components/EffectComponent.hpp>
#include <Components/PhysicsComponent.hpp>
#include <Components/MotionComponent.hpp>
#include <Components/TransformComponent.hpp>

// Same as static in c, local to compilation unit
namespace
{
    const size_t BURST_COOLDOWN_MS = 800;
    const size_t BULLET_COOLDOWN_MS = 100;
    const size_t DAMAGE_EFFECT_TIME = 100;
    const size_t DIRECTION_COOLDOWN_MS = 800;
    const size_t BULLET_DAMAGE = 5;
    const size_t INIT_HEALTH = 100;
    const size_t POINTS_VAL = 10000;
}

Texture Boss1::boss1_texture;

bool Boss1::init(vec2 screen) {
    m_healthbar = &GameEngine::getInstance().getEntityManager()->addEntity<BossHealth>();
    m_healthbar->init(screen, INIT_HEALTH);

    gl_flush_errors();
    auto* sprite = addComponent<SpriteComponent>();
    auto* effect = addComponent<EffectComponent>();
    auto* physics = addComponent<PhysicsComponent>();
    auto* motion = addComponent<MotionComponent>();
    auto* transform = addComponent<TransformComponent>();

    // Load shared texture
    if (!boss1_texture.is_valid())
    {
        if (!boss1_texture.load_from_file(textures_path("boss1.png")))
        {
            fprintf(stderr, "Failed to load Boss1 texture!");
            return false;
        }
    }

    // Loading shaders
    if (!effect->load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;

    if (!sprite->initTexture(&boss1_texture))
        throw std::runtime_error("Failed to initialize bullet sprite");

    if (gl_has_errors())
        return false;

    motion->position = {0.f, 0.f };
    motion->radians = 3.14;
    m_speed = 350.f;
    motion->velocity = {0.f, 0.f}; // TODO switch to using velocity / motion system

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture.
    physics->scale = { -0.4f, 0.4f };

    health = INIT_HEALTH;
    m_is_alive = true;
    points = POINTS_VAL;

    m_burst_count = 0;
    m_burst_cooldown = BURST_COOLDOWN_MS;
    m_bullet_cooldown = 0;
    m_direction_cooldown = 0;

    int randomVal = rand() % 2;
    dir = (randomVal == 0 ? Direction::right : Direction::left);

    return true;
}

void Boss1::destroy() {
    m_healthbar->destroy();

    for (auto bullet : projectiles)
        bullet->destroy();
    projectiles.clear();

    auto* effect = getComponent<EffectComponent>();
    auto* sprite = getComponent<SpriteComponent>();

    effect->release();
    sprite->release();
    ECS::Entity::destroy();
}

void Boss1::update(float ms) {
    m_healthbar->setHealth(health);
    if (m_damage_effect_cooldown > 0)
        m_damage_effect_cooldown -= ms;

    // Update bullets
    for (auto bullet : projectiles)
        bullet->update(ms);

    // Simple health based states, only two states for this first boss
    if (health > 50) state1Update(ms);
    else if (health > 0) state2Update(ms);
    else {
        // death?
    }
}

void Boss1::state1Update(float ms) {
    auto* motion = getComponent<MotionComponent>();

    int w, h;
    glfwGetFramebufferSize(GameEngine::getInstance().getM_window(), &w, &h);
    vec2 screen = { (float)w / GameEngine::getInstance().getM_screen_scale(), (float)h / GameEngine::getInstance().getM_screen_scale() };
    // Set left and right boundaries such that ship doesn't leave the screen
    float lBound = (float)boss1_texture.width * 0.5f;
    float rBound = (float)screen.x - lBound;
    // Adjust x position by speed in direction dir
    int mod = (dir == Direction::right ? 1 : -1);
    float newX = motion->position.x + (float)mod * (ms / 1000) * m_speed;
    // If past a boundary, set position to within the boundary and swap direction
    if (newX >= rBound) {
        newX = rBound;
        dir = Direction::left;
    } else if (newX <= lBound) {
        newX = lBound;
        dir = Direction::right;
    }
    motion->position.x = newX;

    // shoot
    if (m_burst_cooldown > 0) {
        m_burst_cooldown -= ms;
    } else {
        if (m_burst_count >= 3) {
            m_burst_count = 0;
            m_burst_cooldown = BURST_COOLDOWN_MS;
        } else if (m_bullet_cooldown > 0) {
            m_bullet_cooldown -= ms;
        } else {
            ++m_burst_count;
            m_bullet_cooldown = BULLET_COOLDOWN_MS;
            spawnBullet();
            // TODO play sound
        }
    }
}

void Boss1::state2Update(float ms) {
    // Randomly switch directions
    int randomVal = rand() % 100;

    if (m_direction_cooldown > 0)
        m_direction_cooldown -= ms;
    else if (randomVal < 1) {
        int randt = rand() % 2000;
        dir = (dir == Direction::left ? Direction::right : Direction::left);
        m_direction_cooldown = DIRECTION_COOLDOWN_MS + randt;
    }

    state1Update(ms);
}


void Boss1::draw(const mat3 &projection) {
    // Draw boss' bullets
    for (auto bullet : projectiles)
        bullet->draw(projection);

    auto* transform = getComponent<TransformComponent>();
    auto* effect = getComponent<EffectComponent>();
    auto* motion = getComponent<MotionComponent>();
    auto* physics = getComponent<PhysicsComponent>();
    auto* sprite = getComponent<SpriteComponent>();

    transform->begin();
    transform->translate(motion->position);
    transform->scale(physics->scale);
    transform->rotate(motion->radians);
    transform->end();

    float mod = 1;
    if (m_damage_effect_cooldown > 0)
        mod = 1/m_damage_effect_cooldown;

    sprite->draw(projection, transform->out, effect->program, {1.f, mod * 1.f,mod * 1.f});

    m_healthbar->draw(projection);
}

vec2 Boss1::get_position() const {
    auto* motion = getComponent<MotionComponent>();
    return motion->position;
}

void Boss1::set_position(vec2 position) {
    auto* motion = getComponent<MotionComponent>();
    motion->position = position;
}

vec2 Boss1::get_bounding_box() const {
    auto* physics = getComponent<PhysicsComponent>();
    // Returns the local bounding coordinates scaled by the current size of the turtle
    // fabs is to avoid negative scale due to the facing direction.
    return { std::fabs(physics->scale.x) * boss1_texture.width, std::fabs(physics->scale.y) * boss1_texture.height };
}

void Boss1::spawnBullet() {
    auto* motion = getComponent<MotionComponent>();
    Bullet* bullet = &GameEngine::getInstance().getEntityManager()->addEntity<Bullet>();
    if (bullet->init(motion->position, motion->radians+ 3.14f, true, BULLET_DAMAGE)) {
        projectiles.emplace_back(bullet);
    } else {
        throw std::runtime_error("Failed to spawn bullet");
    }
}

void Boss1::addDamage(int damage) {
    m_damage_effect_cooldown = DAMAGE_EFFECT_TIME;
    Boss::addDamage(damage);
}

bool Boss1::collidesWith(const Vamp& vamp) {
    return checkCollision(vamp.get_position(), vamp.get_bounding_box());
}

bool Boss1::collidesWith(Player &player) {
    auto* motion = getComponent<MotionComponent>();

    vec2 playerbox = player.get_bounding_box();

    bool collides = checkCollision(player.get_position(), playerbox);
    if (collides) {
        vec2 player_pos = player.get_position();
        float player_r = std::max(playerbox.x, playerbox.y) * 0.4f;
        float boss_r = std::max(get_bounding_box().x, get_bounding_box().y) * 0.4f;

        auto dist = std::sqrt((float)pow(player_pos.x - motion->position.x,2) + (float)pow(player_pos.y - motion->position.y , 2));
        float overlap = (dist - player_r - boss_r);
        float dx = (player_pos.x - motion->position.x)/dist;
        float dy = (player_pos.y - motion->position.y)/dist;
        player_pos.x -= overlap * dx;
        player_pos.y -= overlap * dy;
        player.set_position(player_pos);


        float boss_mass = boss_r*200;
        float player_mass = player_r*100;

        vec2 player_vel = player.get_velocity();
        vec2 boss_vel = {0,0};
        if (dir == Direction::right) {
            boss_vel.x = m_speed;
        } else {
            boss_vel.x = -m_speed;
        }

        float vbp1 = 2*boss_mass/(player_mass+boss_mass);
        float vbp2 = dot(sub(player_vel, boss_vel), sub(player_pos, motion->position)) / (float)pow(len(sub(player_pos, motion->position)), 2);
        vec2 vb = sub(player_vel, mul(sub(player_pos, motion->position), vbp1*vbp2));

        player.set_velocity(vb);
        player.set_acceleration({0,0});
    }
    return collides;
}

bool Boss1::checkCollision(vec2 pos, vec2 box) const {
    auto* motion = getComponent<MotionComponent>();
    auto* physics = getComponent<PhysicsComponent>();

    vec2 bbox = this->get_bounding_box();

    float dx = motion->position.x - pos.x;
    float dy = motion->position.y - pos.y;
    float d_sq = dx * dx + dy * dy;
    float dr = (std::max(bbox.x, bbox.y)*0.4f) + (std::max(box.x, box.y)*0.4f);
    float r_sq = dr * dr;
    return d_sq < r_sq;

    float other_r = std::max(box.x, box.y);
    float my_r = std::max(bbox.x, bbox.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}