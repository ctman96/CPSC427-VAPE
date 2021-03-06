//
// Created by Cody on 10/17/2019.
//

#include <Engine/GameEngine.hpp>
#include <iostream>
#include "EnemySpawnerSystem.hpp"

void EnemySpawnerSystem::update(float ms) {
    time += ms;
    auto it = level.begin();
    while (it != level.end()) {
        if (it->first <= time) {
            spawnWave(it->second);
            it = level.erase(it);
        } else {
            it++;
        }
    }
}

std::vector<Enemy*> *EnemySpawnerSystem::getEnemies() {
    return &enemies;
}

void EnemySpawnerSystem::reset(Levels::Timeline& levelTimeline) {
    level = levelTimeline;
    time = 0;
    for (auto& enemy : enemies) {
        enemy->destroy();
    }
    enemies.clear();
}

void EnemySpawnerSystem::spawnWave(const Levels::Wave& wave) {
    for (auto &wavit : wave) {
        // std::cout << "spawned" << std::endl;
        Enemy* t = wavit.fn(GameEngine::getInstance().getEntityManager(), wavit.pos, wavit.vel, wavit.dir);
        enemies.emplace_back(t);
    }
}
