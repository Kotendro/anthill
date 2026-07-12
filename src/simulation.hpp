#pragma once

#include <vector>
#include <random>
#include <sol/sol.hpp>
#include "types.hpp"

class Simulation
{
private:
    int width_;
    int height_;
    int ants_count_;
    std::vector<Cell> grid_;
    std::vector<Ant> ants_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;

    sol::state lua_;

    Cell& get_cell(int x, int y);

public:
    const Cell& get_cell (int x, int y) const;
    const int get_width() const {return width_;}
    const int get_height() const {return height_;}
    const std::vector<Cell>& get_grid() const {return grid_;}
    const std::vector<Ant>& get_ants() const {return ants_;}

    Simulation(int width, int height, int ant_count);
    void init();
    void update(float delta_time);
};
