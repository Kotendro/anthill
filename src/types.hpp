#pragma once

#include "types/ant.hpp"
#include "types/pheromone.hpp"

struct Anthill
{
    float relative_x_, relative_y_;
    float food_storage_ = 0.0f;
    // Pheromone pheromone_out_;

    Anthill(float relative_x, float relative_y)
    : relative_x_(relative_x), relative_y_(relative_y) {}

    float get_food(float required) {
        if (food_storage_>required) {
            food_storage_ -= required;
            return required;
        } else {
            float _ = food_storage_;
            food_storage_ = 0.0f;
            return _;
        }
    }
};

struct Food {
    float relative_x_, relative_y_;
    float value_ = 0;
    // Pheromone pheromone_out_;

    Food(float relative_x, float relative_y, float value)
    : relative_x_(relative_x), relative_y_(relative_y), value_(value) {}
};

struct Cell {
    Pheromone pheromone_;

    std::optional<Food> food_;
    std::optional<Anthill> anthill_;
};