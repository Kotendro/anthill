#pragma once

#include "types/ant.hpp"
#include "types/pheromone.hpp"

struct Anthill
{
    float relative_x_, relative_y_;
    int food_storage_ = 0;
    // Pheromone pheromone_out_;

    Anthill(float relative_x, float relative_y)
    : relative_x_(relative_x), relative_y_(relative_y) {}
};

struct Food {
    float relative_x_, relative_y_;
    int count_ = 0;
    // Pheromone pheromone_out_;

    Food(float relative_x, float relative_y, int count)
    : relative_x_(relative_x), relative_y_(relative_y), count_(count) {}
};

struct Cell {
    Pheromone pheromone_;

    std::optional<Food> food_;
    std::optional<Anthill> anthill_;
};