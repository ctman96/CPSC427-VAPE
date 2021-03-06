#pragma once

#include "common.hpp"


class Space : public EntityOld
{
    static Texture bg_texture;
public:
	// Creates all the associated render resources and default transform
	bool init(const char* background);

	// Releases all associated resources
	void destroy();

	void update(float ms);

    void set_position(vec2 position);

	// Renders the water
	void draw(const mat3& projection)override;

	// Player dead time getters and setters
	void set_salmon_dead();
	void reset_salmon_dead_time();
	float get_salmon_dead_time() const;

	// Boss dead time getters and setters
	void set_boss_dead();
	void reset_boss_dead_time();
	float get_boss_dead_time() const;

private:
	// When salmon is alive, the time is set to -1
	float m_dead_time;

	float m_bg_time;

	float m_boss_dead_time;
};
