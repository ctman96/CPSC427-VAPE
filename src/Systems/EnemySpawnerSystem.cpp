//
// Created by Cody on 10/17/2019.
//

#include <Engine/GameEngine.hpp>
#include <iostream>
#include "EnemySpawnerSystem.hpp"

void EnemySpawnerSystem::update(float ms) {
    time += ms; // TODO game speed
    auto it = level.begin();
    while (it != level.end()) {
        if (it->first <= time) {
            Levels::Wave wave = it->second;
            for (auto &wavit : wave) {
                // std::cout << "spawned" << std::endl;
                Turtle* t = wavit.fn(GameEngine::getInstance().getEntityManager(), wavit.pos, wavit.vel, wavit.dir);
                enemies.emplace_back(t);
            }
            it = level.erase(it);
        } else {
            it++;
        }
    }
}

std::vector<Turtle *> *EnemySpawnerSystem::getEnemies() {
    return &enemies;
}

void EnemySpawnerSystem::reset() {
    level = Levels::level1; // TODO
    time = 0;
    enemies.clear();
}