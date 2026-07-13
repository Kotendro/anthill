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
    float spawn_food_chance_ = 0.000f;
    std::vector<Cell> grid_;
    std::vector<Ant> ants_;
    std::vector<Food> food_;
    Anthill anthill_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> angle_;
    std::uniform_real_distribution<float> x_coord_;
    std::uniform_real_distribution<float> y_coord_;

    sol::state lua_;

    Cell& get_cell(int cell_x, int cell_y);
    std::vector<Cell*> get_cells_in_range(float x, float y, float range);
    void try_spawn_food();

public:
    const Cell& get_cell (int cell_x, int cell_y) const;
    const int get_width() const {return width_;}
    const int get_height() const {return height_;}
    const std::vector<Cell>& get_grid() const {return grid_;}
    const std::vector<Ant>& get_ants() const {return ants_;}
    const std::vector<Food>& get_food() const {return food_;}
    const Anthill get_anthill() const {return anthill_;}

    Simulation(int width, int height, int ant_count);
    void init();
    void update(float delta_time);
};
