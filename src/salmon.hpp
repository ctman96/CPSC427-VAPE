#pragma once

#include <map>
#include "common.hpp"
#include <vector>

class Turtle;
class Fish;

class Salmon : public Entity
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, std::map<int, bool> &keyMap, vec2 mouse_position);
	
	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);

	// Returns the current salmon position
	vec2 get_position() const;

	// Returns the current salmon rotation
	float get_rotation()const;

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// Set salmon rotation in radians
	void set_rotation(float radians);

	// Change salmon velocity
	void accelerate(float x, float y);

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Called when the salmon collides with a fish, starts lighting up the salmon
	void light_up();

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	vec2 m_velocity; // Velocity

  	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
};
