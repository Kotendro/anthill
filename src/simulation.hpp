#pragma once

#include <vector>
#include <random>
#include <sol/sol.hpp>
#include <utility>
#include "raylib.h"
#include "types.hpp"

class Simulation
{
private:
    int width_;
    int height_;
    int ants_count_;
    std::vector<Cell> grid_;
    std::vector<Ant> ants_;

    float spawn_food_chance_ = 0.000f;
    int total_food_ = 0;
    const int MAX_FOOD_ = 2;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> start_angle_;
    std::uniform_real_distribution<float> wander_angle_;
    std::uniform_real_distribution<float> x_crd_;
    std::uniform_real_distribution<float> y_crd_;
    std::uniform_int_distribution<int> count_;

    sol::state lua_;

    Cell& get_cell(Vector2 crd);
    void set_pheromone_in_radius(Vector2 center, float radius, PheromoneType ptype);
    void try_spawn_food();
    void spawn_food();
    Vector2 spawn_anthill();

public:
    const Cell& get_cell (Vector2 crd) const;
    const int get_width() const {return width_;}
    const int get_height() const {return height_;}
    const std::vector<Cell>& get_grid() const {return grid_;}
    const std::vector<Ant>& get_ants() const {return ants_;}

    Simulation(int width, int height, int ant_count);
    void init();
    void update(float delta_time);
};
