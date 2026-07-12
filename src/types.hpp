#pragma once

struct Pheromone {
    // intensity_ = 0.0f - 1.0f
    float food_intensity_; 
    float home_intensity_;

    Pheromone(float food_intensity=0.0f, float home_intensity=0.0f)
    : food_intensity_(food_intensity), home_intensity_(home_intensity) {}
};

struct Ant
{
    float x_, y_;
    float angle_; // В градусах
    float speed_=10.0f;
    bool has_food=false;
    Pheromone pheromone_out_ {0.0f, 0.0f};

    Ant(float x, float y, float angle)
    : x_(x), y_(y), angle_(angle) {}
};

struct Anthill
{
    float x_, y_;
    int food_storage_ = 0;
    Pheromone pheromone_ {0.0f, 1.0f};

    Anthill(float x, float y)
    : x_(x), y_(y) {}
};


struct Cell {
    Pheromone pheromone_ {0.0f, 0.0f};
    int food_units_ = 0;
};