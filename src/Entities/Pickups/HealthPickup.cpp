//
// Created by Andrea Park on 2019-12-05.
//

#include "HealthPickup.hpp"

#include <cmath>
#include <Components/SpriteComponent.hpp>
#include <Components/EffectComponent.hpp>
#include <Components/PhysicsComponent.hpp>
#include <Components/MotionComponent.hpp>
#include <Components/TransformComponent.hpp>
#include <Components/EnemyComponent.hpp>
#include <Entities/Weapons/Weapon.hpp>

Texture HealthPickup::pickup_texture;

bool HealthPickup::init(vec2 position) {
    auto* sprite = addComponent<SpriteComponent>();
    auto* effect = addComponent<EffectComponent>();
    auto* physics = addComponent<PhysicsComponent>();
    auto* motion = addComponent<MotionComponent>();
    auto* transform = addComponent<TransformComponent>();
    auto* enemy = addComponent<EnemyComponent>();

    // Load shared texture
    if (!pickup_texture.is_valid())
    {
        if (!pickup_texture.load_from_file(textures_path("health_icon.png")))
        {
            fprintf(stderr, "Failed to load pickup enemy texture!");
            return false;
        }
    }

    // Loading shaders
    if (!effect->load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;


    if (!sprite->initTexture(&pickup_texture))
        throw std::runtime_error("Failed to initialize turtle sprite");

    if (gl_has_errors())
        return false;

    motion->radians = 0.f;
    motion->velocity = { 0.f, 20.f };
    motion->position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture.
    physics->scale = { -1.f, 1.f };

    return true;
}

void HealthPickup::update(float ms) {
}

void HealthPickup::draw(const mat3 &projection) {
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

void HealthPickup::destroy() {
    auto* effect = getComponent<EffectComponent>();
    auto* sprite = getComponent<SpriteComponent>();

    effect->release();
    sprite->release();
    ECS::Entity::destroy();
}

vec2 HealthPickup::get_position() const {
    auto* motion = getComponent<MotionComponent>();
    return motion->position;
}

bool HealthPickup::collides_with(const Player &player) {
    auto* physics = getComponent<PhysicsComponent>();
    auto* motion = getComponent<MotionComponent>();

    float dx = motion->position.x - player.get_position().x;
    float dy = motion->position.y - player.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(player.get_bounding_box().x, player.get_bounding_box().y);
    float my_r = std::max(pickup_texture.width * physics->scale.x * 0.55f, pickup_texture.height * physics->scale.y * 0.55f);
    float r = std::max(other_r, my_r);
    r *= 0.65f;
    if (d_sq < r * r)
        return true;
    return false;
}

void HealthPickup::applyEffect(Player& player) {
    player.reset_health();
}

vec2 HealthPickup::get_bounding_box() const
{
    auto* physics = getComponent<PhysicsComponent>();

    // Returns the local bounding coordinates scaled by the current size of the turtle
    // fabs is to avoid negative scale due to the facing direction.
    return { std::fabs(physics->scale.x) * pickup_texture.width, std::fabs(physics->scale.y) * pickup_texture.height };
}

bool HealthPickup::isOffScreen(const vec2& screen) {
    float h = get_bounding_box().y / 2;
    return (get_position().y - h > screen.y);
}

const char* HealthPickup::get_png() const {
    return textures_path("health_icon.png");
}

bool HealthPickup::isWeapon() {
    return false;
}