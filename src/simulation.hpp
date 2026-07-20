#pragma once

#include <vector>
#include <random>
#include <sol/sol.hpp>
#include <utility>
#include <raylib.h>

#include "types.hpp"

class Simulation
{
private:
    std::mt19937 rng_;

    Grid grid_;
    std::vector<Ant> ants_;

    std::uniform_real_distribution<float> start_angle_;

    sol::state& lua_;

public:
    const std::vector<Ant>& get_ants() const {return ants_;}
    const Grid& get_grid() const {return grid_;}

    Simulation(sol::state& lua);
    void init();
    void update(float delta_time);
    void reset();
};
