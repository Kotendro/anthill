#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>



Simulation::Simulation(int width, int height, int ants_count) 
: width_(width), height_(height), ants_count_(ants_count), rng_(std::random_device{}()), angle_(-90.0f, 90.0f),
anthill_(10, 10), x_coord_(0, width), y_coord_(0, height)
{
    grid_.resize(width_*height_);
    ants_.reserve(ants_count_);

    lua_.script_file("scripts/ant.lua");
}

void Simulation::init() {
    // Ants
    for (int i=0; i < ants_count_; i++) {
        Ant ant{anthill_.x_, anthill_.y_, 0};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    // Cells
    for (Cell& cell : grid_) {
        cell.pheromone_.evaporate(delta_time);
    }

    // Food
    try_spawn_food();
    for (Food& food : food_) {
        Cell& cell = get_cell(food.x_, food.y_);
        cell.pheromone_.set(PheromoneType::Food, 1.0f);
    }

    // Ants
    for (Ant& ant : ants_) {
        ant.angle_ += angle_(rng_) * delta_time * 10.f;
        
        float radians = ant.angle_ * (std::numbers::pi / 180);
        ant.x_ += std::cos(radians) * ant.speed_ * delta_time;
        ant.y_ += std::sin(radians) * ant.speed_ * delta_time;

        if (ant.x_ < -1) ant.x_ = width_ - 1;
        if (ant.x_ >= width_) ant.x_ = 0;
        if (ant.y_ < -1) ant.y_ = height_ - 1;
        if (ant.y_ >= height_) ant.y_ = 0;

        Cell& cell = get_cell(ant.x_, ant.y_);
        cell.pheromone_.set(PheromoneType::Home, 0.9f);
    }

    // Anthill
    for (float dx=-1.0f; dx<2.0f; dx+=1.0f) {
        for (float dy=-1.0f; dy<2.0f; dy+=1.0f) {
            Cell& cell = get_cell(anthill_.x_+dx, anthill_.y_+dy);
            cell.pheromone_.set(PheromoneType::Home, 1.0f);
        }
    }
}

Cell& Simulation::get_cell(int x, int y) {
    return grid_.at(width_*y + x);
}

const Cell& Simulation::get_cell(int x, int y) const {
    return grid_.at(width_*y + x);
}

void Simulation::try_spawn_food() {
    std::bernoulli_distribution distribution(spawn_food_chance_);
    spawn_food_chance_ = std::clamp(spawn_food_chance_+0.00001f, 0.0f, 0.1f);

    if (distribution(rng_)) {
        float x = x_coord_(rng_);
        float y = y_coord_(rng_);
        Food food {x, y};
        food_.push_back(food);
    }
}