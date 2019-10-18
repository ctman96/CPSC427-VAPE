//
// Created by matte on 10/15/2019.
//

#include "VampCharge.hpp"


Texture VampCharge::vamp_charge_texture;

bool VampCharge::init(vec2 position) {
    // Load shared texture
    if (!vamp_charge_texture.is_valid())
    {
        if (!vamp_charge_texture.load_from_file(textures_path("vamp_charge.png")))
        {
            fprintf(stderr, "Failed to load vamp_charge texture!");
            return false;
        }
    }

    // The position corresponds to the center of the texture
    float wr = vamp_charge_texture.width * 1.0f;
    float hr = vamp_charge_texture.height * 1.0f;

    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 0.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 1.f, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 1.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 0.f, 0.f };

    // counterclockwise as it's the default opengl front winding direction
    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertexd + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;


    physics.scale.x = 0.25f;
    physics.scale.y = 0.25f;
    this->motion.position.x = position.x;
    this->motion.position.y = position.y;

    return true;
}

void VampCharge::update(float ms) {
}

void VampCharge::draw(const mat3 &projection) {
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT

    for (int i = 0; i < 4 * charge; i++) {
        transform.begin();
        vec2 offset = {i/2.f * 5.f, 0.f};
        if (i%2 == 0) {
            offset = {i/2.f * - 5.f, 0.f};
        }
        offset.x += motion.position.x;
        offset.y += motion.position.y;
        transform.translate(offset);
        transform.scale(physics.scale);
        transform.end();

        // Setting shaders
        glUseProgram(effect.program);

        // Enabling alpha channel for textures
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        // Getting uniform locations for glUniform* calls
        GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
        GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
        GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

        // Setting vertices and indices
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

        // Input data location as in the vertex buffer
        GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
        GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
        glEnableVertexAttribArray(in_position_loc);
        glEnableVertexAttribArray(in_texcoord_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
        glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

        // Enabling and binding texture to slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vamp_charge_texture.id);

        // Setting uniform values to the currently bound program
        glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
        float color[] = { 0.5f, 1.f, 0.5f };
        glUniform3fv(color_uloc, 1, color);
        glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

        // Drawing!
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    }
}

void VampCharge::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void VampCharge::setVampCharge(int charge) {
    VampCharge::charge = charge;
}
