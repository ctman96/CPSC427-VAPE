//
// Created by Cody on 11/7/2019.
//

#include <Components/SpriteComponent.hpp>
#include <Components/EffectComponent.hpp>
#include <Components/PhysicsComponent.hpp>
#include <Components/TransformComponent.hpp>
#include <Engine/GameEngine.hpp>
#include "Laser.hpp"


constexpr int NUM_SEGMENTS = 12;
constexpr int MAX_PARTICLES = 5000;
constexpr int LASER_WIDTH = 20;

bool Laser::init(vec2 position, float rotation) {
    gl_flush_errors();
    auto* effect = addComponent<EffectComponent>();

    // Loading shaders
    if (!effect->load_from_file(shader_path("laser.vs.glsl"), shader_path("laser.fs.glsl")))
        return false;

    if (gl_has_errors())
        return false;


    std::vector<GLfloat> screen_vertex_buffer_data;
    constexpr float z = -0.1;

    for (int i = 0; i < NUM_SEGMENTS; i++) {
        screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(z);

        screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
        screen_vertex_buffer_data.push_back(z);

        screen_vertex_buffer_data.push_back(0);
        screen_vertex_buffer_data.push_back(0);
        screen_vertex_buffer_data.push_back(z);
    }

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size()*sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

    if (gl_has_errors())
        return false;


    m_spr = &GameEngine::getInstance().getEntityManager()->addEntity<LaserBeamSprite>();
    m_spr->init(position);


    m_origin = position;
    m_erase_on_collide = false;
    m_rotation = rotation;
    m_rotationTarget = rotation;
    m_chargeTimer = 0;
    m_fireTimer = 0;
    m_state = laserState::off;
    return true;
}

void Laser::destroy() {
    m_spr->destroy();

    auto* effect = getComponent<EffectComponent>();

    effect->release();
    ECS::Entity::destroy();
}

void Laser::update(float ms) {

    // Update timers
    if (m_state == laserState::primed){
        if (m_chargeTimer > 0)
            m_chargeTimer -= ms;
        else {
            m_chargeTimer = 0;
            m_state = laserState::firing;
        }
    }
    if (m_state == laserState::firing) {
        if (m_fireTimer > 0)
            m_fireTimer -= ms;
        else {
            m_fireTimer = 0;
            m_state = laserState::off;
        }


        // Rotate towards target
        if (m_rotationTarget != m_rotation) {
            float step = 0.1f * (ms / 1000);
            if (m_rotationTarget < m_rotation) {
                m_rotation -= step;
                if (m_rotation < m_rotationTarget)
                    m_rotation = m_rotationTarget;
            } else {
                m_rotation += step;
                if (m_rotation > m_rotationTarget)
                    m_rotation = m_rotationTarget;
            }
        }
    }


    // Particle Effects
    spawn();

    for (auto& particle : m_particles) {
        particle.life -= ms;
        particle.position.x += particle.velocity.x * (ms / 1000);
        particle.position.y += particle.velocity.y * (ms / 1000);
    }

    auto particle_it = m_particles.begin();
    while (particle_it != m_particles.end()) {
        if ((*particle_it).life <= 0
            || (*particle_it).position.y > 1000 // TODO screen size
            || (*particle_it).position.y < 0
            || (*particle_it).position.x < 0
            || (*particle_it).position.x > 800) {
            particle_it = m_particles.erase(particle_it);
            continue;
        } else {
            ++particle_it;
        }
    }
}

void Laser::spawn() {
    int n = 0;
    vec2 vel = {2000*sinf(m_rotation), 2000*cosf(m_rotation)};

    float xrange = (float)LASER_WIDTH/2;
    if (m_state == laserState::firing) {
        return; // TODO: Any particles while firing?
    } else if (m_state == laserState::primed) {
        n = 10;
        xrange = 1;
    }

    vec2 xy1 = {m_origin.x + (xrange*sinf(m_rotation - 1.5708f)), m_origin.y - (10*cosf(m_rotation))};
    vec2 xy2 = {m_origin.x + (xrange*sinf(m_rotation + 1.5708f)), m_origin.y + (10*cosf(m_rotation))};
    float xmin = std::min(xy1.x, xy2.x);
    float xmax = std::max(xy1.x, xy2.x);
    float ymin = std::min(xy1.y, xy2.y);
    float ymax = std::max(xy1.y, xy2.y);

    for (int i = 0; i < n && m_particles.size() < MAX_PARTICLES; i++) {
        m_particles.emplace_back();
        Particle& p = m_particles.back();

        float randx = xmin;
        if (xmin != xmax)
            randx = rand() % (int)(xmax-xmin) + (int)xmin;
        float randy = ymin;
        if (ymin != ymax)
            randy = rand() % (int)(ymax-ymin) + (int)ymin;
        p.position = {randx, randy};
        p.velocity = vel;
        p.life = 2000;
        p.color = {1.f,1.f,1.f,0.5f};
        if (m_state == laserState::firing) {
            float mod = (std::fabs(randx - m_origin.x) / LASER_WIDTH/2);
            p.color = {1.f * mod, 1.f * mod, 1.f, 0.8f};
        }
    }
}

void Laser::draw(const mat3 &projection) {
    if (m_state == laserState::firing)
        m_spr->draw(projection, m_rotation);
    auto* effect = getComponent<EffectComponent>();

    // Setting shaders
    glUseProgram(effect->program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Getting uniform locations
    GLint projection_uloc = glGetUniformLocation(effect->program, "projection");
    GLint color_uloc = glGetUniformLocation(effect->program, "color");
    GLint center_uloc = glGetUniformLocation(effect->program, "laser_center");

    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    float center[] = {m_origin.x, m_origin.y};
    glUniform2fv(center_uloc, 1, center);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Draw the screen texture on the geometry
    // Setting vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Mesh vertex positions
    // Bind to attribute 0 (in_position) as in the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(0, 0);

    // Load up particles into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(Particle), m_particles.data(), GL_DYNAMIC_DRAW);

    // particles translations
    // Bind to attribute 1 (in_translate) as in vertex shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, position));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color));
    glVertexAttribDivisor(2, 1);

    // Draw using instancing
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
    glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS*3, m_particles.size());

    // Reset divisor
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
}

vec2 Laser::get_position() const {
    return m_origin; // TODO?
}

// https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
bool lineCircleCollision(vec2 start, vec2 end, vec2 center, float radius) {
    vec2 d = sub(end, start);
    vec2 f = sub(start, center);

    float a = dot(d, d);
    float b = 2*dot(f, d);
    float c = dot(f, f) - radius*radius;

    float discriminant = b*b-4*a*c;
    if (discriminant < 0) {
        return false;
    } else {
        discriminant = sqrtf(discriminant);

        float t1 = (-b - discriminant) / (2*a);
        float t2 = (-b + discriminant) / (2*a);

        if (t1 >= 0 && t1 <= 1) {
            return true;
        }

        return t2 >= 0 && t2 <= 1;
    }
}

bool Laser::collides_with(const Player &player) {
    if (m_state != laserState::firing)
        return false;

    vec2 playerpos = player.get_position();
    vec2 playerbox = player.get_bounding_box();
    float playerr = std::max(playerbox.x/2, playerbox.y/2)*0.6f;

    float lx = m_origin.x - LASER_WIDTH/3;
    vec2 p00 = {lx, m_origin.y};
    vec2 p01 = { lx + 2000*sinf(m_rotation), m_origin.y + 2000*cosf(m_rotation)};

    float rx = m_origin.x + LASER_WIDTH/3;
    vec2 p10 = {rx, m_origin.y};
    vec2 p11 = {rx + 2000*sinf(m_rotation), m_origin.y + 2000*cosf(m_rotation)};

    float w = playerbox.x/2;
    float h = playerbox.y/2;
    vec2 tl = {playerpos.x - w, playerpos.y - h};
    vec2 br = {playerpos.x + w, playerpos.y + h};

    return lineCircleCollision(p00, p01, playerpos, playerr) || lineCircleCollision(p10, p11, playerpos, playerr);
    /*
    return (CohenSutherlandLineClipAndDraw(p00, p01, tl, br) ||
            CohenSutherlandLineClipAndDraw(p10, p11, tl, br));
            */
}

bool Laser::collides_with(const Enemy &turtle) {
    return false;
}

bool Laser::collides_with(const Boss &boss) {
    return false;
}

bool Laser::isOffScreen(const vec2 &screen) {
    return false;
}

void Laser::fire(float chargeDur, float fireDur) {
    m_state = laserState::primed;
    m_chargeTimer = chargeDur;
    m_fireTimer = fireDur;
}

void Laser::setRotationTarget(vec2 position) {
    setRotationTarget(atan2f(position.x - m_origin.x, position.y - m_origin.y));
}

void Laser::set_position(vec2 position) {
    m_origin = position;
    if (m_spr != nullptr)
        m_spr->set_position(position);
}