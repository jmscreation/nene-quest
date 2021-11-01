#ifndef CMDITEM_HPP_INCLUDED
#define CMDITEM_HPP_INCLUDED

#include <random>

#include "parserFunction.hpp"

class cmdHealth : public ParserFunction {
   public:
    void initialize(LevelManager *manager) {
        this->commands.push_back("health");
        ParserFunction::manager = manager;
    }

    void exec(CustomMap *args) {
        auto strength = std::stoi(args->getOrDefault("strength", "50"));
        auto x = std::stoi(args->getOrDefault("x", "-1"));
        auto y = std::stoi(args->getOrDefault("y", "-1"));
        auto randx_max = std::stoi(args->getOrDefault("randx_max", "500"));
        auto randx_min = std::stoi(args->getOrDefault("randx_min", "0"));
        auto randy_max = std::stoi(args->getOrDefault("randy_max", "500"));
        auto randy_min = std::stoi(args->getOrDefault("randy_min", "0"));
        if (x == -1) x = rand() * (randx_max - randx_min) + randx_min;

        if (y == -1) y = rand() * (randy_max - randy_min) + randy_min;

        ParserFunction::manager->spawnOnigiri(x, y, strength);
    }
};

#endif
