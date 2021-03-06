//
// Created by Andy Kim on 12/05/2019.
//

#include "Boss3Clone.hpp"

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

Texture Boss3Clone::texture;
using namespace std;

namespace
{
    const size_t BURST_COOLDOWN_MS = 1000;
    const size_t BULLET_COOLDOWN_MS = 200;
    const size_t BULLET_DAMAGE = 5;
	const size_t STUN_DURATION_MS = 1300;
}

bool Boss3Clone::init(vec2 pos, vec2 disp) {
    auto* sprite = addComponent<SpriteComponent>();
    auto* effect = addComponent<EffectComponent>();
    auto* physics = addComponent<PhysicsComponent>();
    auto* motion = addComponent<MotionComponent>();
    auto* enemy = addComponent<EnemyComponent>();
    auto* transform = addComponent<TransformComponent>();

    // Load shared texture
    if (!texture.is_valid())
    {
        if (!texture.load_from_file(textures_path("boss3clone.png")))
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
    motion->velocity = {0.f, 0.f};
	motion->position = pos;
	m_curr_state = CloneState::moving;
	m_prev_state = CloneState::moving;
	m_target_pos = disp;
	m_is_active = true;

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture.
    physics->scale = { -0.2f, 0.2f };

    m_burst_count = 0;
    m_burst_cooldown = BURST_COOLDOWN_MS;
    m_bullet_cooldown = 0;
	m_stun_duration = 0;

    return true;
}

void Boss3Clone::destroy() {
    auto* effect = getComponent<EffectComponent>();
    auto* sprite = getComponent<SpriteComponent>();

    effect->release();
    sprite->release();
    ECS::Entity::destroy();
}

void Boss3Clone::update(float ms) {
	auto* motion = getComponent<MotionComponent>();
	if (!m_is_active) {
		return;
	}
	if (m_curr_state == CloneState::moving) {
		vec2 displacement = move_to(m_target_pos,120.f);
		if (abs(displacement.x) < 0.5f && abs(displacement.y) < 0.5f) {
			m_curr_state = CloneState::attack;
			motion->velocity = { 0.f,0.f };
			motion->radians = M_PI;
		}
	}
	else if (m_curr_state == CloneState::attack) {
		if (m_is_lead) {
			if (m_burst_cooldown > 0) {
				m_burst_cooldown -= ms;
			}
			else {
				if (m_burst_count >= 3) {
					m_burst_count = 0;
					m_burst_cooldown = BURST_COOLDOWN_MS;
				}
				else if (m_bullet_cooldown > 0) {
					m_bullet_cooldown -= ms;
				}
				else {
					++m_burst_count;
					m_bullet_cooldown = BULLET_COOLDOWN_MS;
					spawnBullet();
				}
			}
			float dx = player_pos.x - motion->position.x;
			motion->velocity.x = dx / 3.f;
			if (motion->position.y < 300.f)
				motion->velocity.y = 90.f;
			else
				motion->velocity.y = 0.f;
		}
		else {
			move_to(player_pos,120.f);	
		}
	}
	else if (m_curr_state == CloneState::stunned) {
		if (m_stun_duration < 0.f) {
			m_curr_state = m_prev_state;
			motion->velocity = { 0.f,0.f };
			motion->radians = M_PI;
		} else {
			if (motion->position.y > 0.f) 
				motion->velocity = { 0.f,-110.f };
			else 
				m_stun_duration = 0.f;
			motion->radians += 0.1f;
			m_stun_duration -= ms;
		}
	}
}

void Boss3Clone::draw(const mat3 &projection) {
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


vec2 Boss3Clone::get_position() const {
    auto* motion = getComponent<MotionComponent>();
    return motion->position;
}

void Boss3Clone::set_position(vec2 position) {
    auto* motion = getComponent<MotionComponent>();
    motion->position = position;
}

vec2 Boss3Clone::get_bounding_box() const {
    auto* physics = getComponent<PhysicsComponent>();

    // Returns the local bounding coordinates scaled by the current size of the turtle
    // fabs is to avoid negative scale due to the facing direction.
    return { std::fabs(physics->scale.x) * texture.width, std::fabs(physics->scale.y) * texture.height };
}
bool Boss3Clone::collidesWith(Player& player) {
	bool collides = checkCollision(player.get_position(), player.get_bounding_box());
	if (collides)
		stun();
	return collides;
}
bool Boss3Clone::collidesWith(Vamp& vamp) {
	bool collides = checkCollision(vamp.get_position(), vamp.get_bounding_box());
	if (collides)
		stun();
	return collides;
}

bool Boss3Clone::checkCollision(vec2 pos, vec2 box) const {
	auto* motion = getComponent<MotionComponent>();
	auto* physics = getComponent<PhysicsComponent>();

	vec2 bbox = this->get_bounding_box();

	float dx = motion->position.x - pos.x;
	float dy = motion->position.y - pos.y;
	float d_sq = dx * dx + dy * dy;
	float dr = (std::max(bbox.x, bbox.y) * 0.4f) + (std::max(box.x, box.y) * 0.4f);
	float r_sq = dr * dr;
	return d_sq < r_sq;
}

void Boss3Clone::set_velocity(vec2 velocity) {
    auto* motion = getComponent<MotionComponent>();
    motion->velocity.x = velocity.x;
    motion->velocity.y = velocity.y;
}

void Boss3Clone::spawnBullet() {
    auto & projectiles = GameEngine::getInstance().getSystemManager()->getSystem<ProjectileSystem>();
    auto* motion = getComponent<MotionComponent>();
    Bullet* bullet = &GameEngine::getInstance().getEntityManager()->addEntity<Bullet>();
    if (bullet->init(motion->position, motion->radians + 3.14f, true, BULLET_DAMAGE)) {
		bullet->set_speed_slow();
        projectiles.hostile_projectiles.emplace_back(bullet);
    } else {
        throw std::runtime_error("Failed to spawn bullet");
    }
}

vec2 Boss3Clone::move_to(vec2 target, float speed) {
	auto* motion = getComponent<MotionComponent>();


	float dy = target.y - motion->position.y;
	float dx = target.x - motion->position.x;

	float angle = atan2(dx, dy);
	motion->radians = angle - M_PI;

	motion->velocity = { speed * sin(angle), speed * cos(angle) };
	return { dx,dy };
}

void Boss3Clone::stun() {
	if (m_curr_state != CloneState::stunned && m_curr_state != CloneState::moving) {
		m_prev_state = m_curr_state;
		m_curr_state = CloneState::stunned;
		m_stun_duration = STUN_DURATION_MS;
	}
}

void Boss3Clone::set_lead() {
	m_is_lead = true;
}

void Boss3Clone::shutdown(vec2 master_pos) {
	m_is_active = false;
	move_to(master_pos, 120.f);
}