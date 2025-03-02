#ifndef CMDWEAPON_HPP_INCLUDED
#define CMDWEAPON_HPP_INCLUDED

#include <random>

#include "parserFunction.hpp"

class cmdWeapon : public ParserFunction {
   public:
    void initialize(LevelManager *manager) {
        this->commands.push_back("weapon");
        ParserFunction::manager = manager;
    }

    void exec(CustomMap *args) {
        auto item = args->getOrDefault("item", "none");
        auto x = std::stoi(args->getOrDefault("x", "-1"));
        auto y = std::stoi(args->getOrDefault("y", "-1"));
        auto randx_max = std::stoi(args->getOrDefault("randx_max", "100"));
        auto randx_min = std::stoi(args->getOrDefault("randx_min", "0"));
        auto randy_max = std::stoi(args->getOrDefault("randy_max", "100"));
        auto randy_min = std::stoi(args->getOrDefault("randy_min", "0"));
        if (x == -1) x = rand() * (randx_max - randx_min) + randx_min;

        if (y == -1) y = rand() * (randy_max - randy_min) + randy_min;

        ParserFunction::manager->spawnWeapon(x, y, item);
    }
};

#endif
