//
// Created by Cody on 11/19/2019.
//

#ifndef VAPE_ENTERCONTINUE_HPP
#define VAPE_ENTERCONTINUE_HPP


#include <common.hpp>

class EnterContinue : public EntityOld {
    static Texture enter_texture;

public:
    // Creates all the associated render resources and default transform
    bool init();

    // Releases all the associated resources
    void destroy();

    void update(float ms);

    void draw(const mat3& projection) override;

    vec2 get_position()const;

    void set_position(vec2 position);

    bool isActive();

    void set_activity(bool set);

private:
    bool m_is_active;
};


#endif //VAPE_ENTERCONTINUE_HPP
