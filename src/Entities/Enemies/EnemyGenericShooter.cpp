//
// Created by Tanha Kabir on 2019-11-11.
//

#include "EnemyGenericShooter.hpp"

#include <cmath>
#include <Components/SpriteComponent.hpp>
#include <Components/EffectComponent.hpp>
#include <Components/PhysicsComponent.hpp>
#include <Components/MotionComponent.hpp>
#include <Components/TransformComponent.hpp>
#include <Entities/Projectiles and Damaging/bullet.hpp>
#include <Engine/GameEngine.hpp>
#include <Components/EnemyComponent.hpp>
#include <Systems/ProjectileSystem.hpp>

Texture EnemyGenericShooter::texture;
using namespace std;

namespace
{
    const size_t POINTS_VAL = 250;
    const size_t BURST_COOLDOWN_MS = 1000;
    const size_t BULLET_COOLDOWN_MS = 200;
    const size_t BULLET_DAMAGE = 5;
}

bool EnemyGenericShooter::init() {
    auto* sprite = addComponent<SpriteComponent>();
    auto* effect = addComponent<EffectComponent>();
    auto* physics = addComponent<PhysicsComponent>();
    auto* motion = addComponent<MotionComponent>();
    auto* enemy = addComponent<EnemyComponent>();
    auto* transform = addComponent<TransformComponent>();

    // Load shared texture
    if (!texture.is_valid())
    {
        if (!texture.load_from_file(textures_path("turtle2.png")))
        {
            fprintf(stderr, "Failed to load generic shooter texture!");
            return false;
        }
    }

    // Loading shaders
    if (!effect->load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;


    if (!sprite->initTexture(&texture))
        throw runtime_error("Failed to initialize health sprite");

    if (gl_has_errors())
        return false;

    motion->radians = M_PI;
    motion->velocity = {0.f, 180.f};

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture.
    physics->scale = { -0.28f, 0.28f };

    m_burst_count = 0;
    m_burst_cooldown = BURST_COOLDOWN_MS;
    m_bullet_cooldown = 0;
    points = POINTS_VAL;

    return true;
}

void EnemyGenericShooter::destroy() {
    auto* effect = getComponent<EffectComponent>();
    auto* sprite = getComponent<SpriteComponent>();

    effect->release();
    sprite->release();
    ECS::Entity::destroy();
}

void EnemyGenericShooter::update(float ms) {
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
        }
    }
}

void EnemyGenericShooter::draw(const mat3 &projection) {
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

    sprite->draw(projection, transform->out, effect->program);
}

vec2 EnemyGenericShooter::get_position() const {
    auto* motion = getComponent<MotionComponent>();
    return motion->position;
}

void EnemyGenericShooter::set_position(vec2 position) {
    auto* motion = getComponent<MotionComponent>();
    motion->position = position;
}

vec2 EnemyGenericShooter::get_bounding_box() const {
    auto* physics = getComponent<PhysicsComponent>();

    // Returns the local bounding coordinates scaled by the current size of the turtle
    // fabs is to avoid negative scale due to the facing direction.
    return { std::fabs(physics->scale.x) * texture.width, std::fabs(physics->scale.y) * texture.height };
}

void EnemyGenericShooter::set_velocity(vec2 velocity) {
    auto* motion = getComponent<MotionComponent>();
    motion->velocity.x = velocity.x;
    motion->velocity.y = velocity.y;
}

void EnemyGenericShooter::spawnBullet() {
    auto & projectiles = GameEngine::getInstance().getSystemManager()->getSystem<ProjectileSystem>();
    auto* motion = getComponent<MotionComponent>();
    Bullet* bullet = &GameEngine::getInstance().getEntityManager()->addEntity<Bullet>();
    if (bullet->init(motion->position, motion->radians + 3.14f, true, BULLET_DAMAGE)) {
        projectiles.hostile_projectiles.emplace_back(bullet);
    } else {
        throw std::runtime_error("Failed to spawn bullet");
    }
}