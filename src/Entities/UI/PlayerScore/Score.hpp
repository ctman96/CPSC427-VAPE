//
// Created by matte on 2019-12-05.
//

#ifndef VAPE_SCORE_HPP
#define VAPE_SCORE_HPP

#include <Entities/UI/Text.hpp>
#include "common.hpp"
#include "Engine/ECS/ECS.hpp"

class Score {

public:
    bool init(vec2 position, Font font);
    void update(float ms) override;
    void draw(const mat3& projection) override;
    void destroy() override;

    void setScore(int score);

private:
    int score = 0;
    Text score_text;
    Font score_font;
};



#endif //VAPE_SCORE_HPP
