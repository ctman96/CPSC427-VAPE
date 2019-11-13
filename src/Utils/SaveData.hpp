//
// Created by Cody on 11/12/2019.
//

#ifndef VAPE_SAVEDATA_HPP
#define VAPE_SAVEDATA_HPP

#include <string>
#include <set>

constexpr char defaultName[] = "Vaypur";

struct leaderboardEntry {
    std::string name;
    unsigned int points;

    bool operator<(const leaderboardEntry &rhs) const {
        return points < rhs.points;
    }

    bool operator>(const leaderboardEntry &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const leaderboardEntry &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const leaderboardEntry &rhs) const {
        return !(*this < rhs);
    }
};

void saveScore(unsigned int points, const std::string&name = std::string(defaultName));

#endif //VAPE_SAVEDATA_HPP
