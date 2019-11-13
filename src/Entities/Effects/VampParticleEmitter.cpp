//
// Created by matte on 2019-11-12.
//

#include "VampParticleEmitter.hpp"
#include <cmath>
#include <iostream>

constexpr int NUM_SEGMENTS = 12;

bool VampParticleEmitter::init()
{
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
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size()*sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("vampParticle.vs.glsl"), shader_path("vampParticle.fs.glsl")))
        return false;

    return true;
}

void VampParticleEmitter::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &m_instance_vbo);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);

    m_particles.clear();
}

void VampParticleEmitter::update(float ms)
{
    // TODO
}

void VampParticleEmitter::draw(const mat3& projection)
{
    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Getting uniform locations
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLint color_uloc = glGetUniformLocation(effect.program, "color");

    // Pebble color
    // TODO
    float r = 0.8f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.2)));
    float g = 0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.9)));
    float color[] = { r, g, 0.4f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Draw the screen texture on the geometry
    // Setting vertices
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    // Mesh vertex positions
    // Bind to attribute 0 (in_position) as in the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(0, 0);

    // Load up pebbles into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(VampParticle), m_particles.data(), GL_DYNAMIC_DRAW);

    // Pebble translations
    // Bind to attribute 1 (in_translate) as in vertex shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VampParticle), (GLvoid*)offsetof(VampParticle, position));
    glVertexAttribDivisor(1, 1);

    // Pebble radii
    // Bind to attribute 2 (in_scale) as in vertex shader
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VampParticle), (GLvoid*)offsetof(VampParticle, radius));
    glVertexAttribDivisor(2, 1);

    // Draw using instancing
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
    glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS*3, m_particles.size());

    // Reset divisor
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
}