#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>



Simulation::Simulation(int width, int height, int ants_count) 
: width_(width), height_(height), ants_count_(ants_count), rng_(std::random_device{}()), angle_(-90.0f, 90.0f),
anthill_(10.5, 10.5), x_coord_(0, width), y_coord_(0, height)
{
    grid_.resize(width_*height_);
    ants_.reserve(ants_count_);

    try {
        lua_.script_file("scripts/ant.lua");
    } catch (const sol::error& e) {
        std::cerr << "Lua script error" << std::endl;
    }
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
        auto cells = get_cells_in_range(food.x_, food.y_, 0.5f);
        for (Cell* cell_p : cells) {
            cell_p->pheromone_.set(PheromoneType::Food, 1.0f);
        }
    }

    // Ants
    for (Ant& ant : ants_) {
        ant.angle_ += angle_(rng_) * delta_time * 10.f;
        
        float radians = ant.angle_ * (std::numbers::pi / 180);
        ant.x_ += std::cos(radians) * ant.speed_ * delta_time;
        ant.y_ += std::sin(radians) * ant.speed_ * delta_time;

        if (ant.x_ < 0) ant.x_ += width_;
        if (ant.x_ >= width_) ant.x_ -= width_;
        if (ant.y_ < 0) ant.y_ += height_;
        if (ant.y_ >= height_) ant.y_ -= height_;

        Cell& cell = get_cell(ant.x_, ant.y_);
        cell.pheromone_.set(PheromoneType::Home, 0.9f);
    }

    // Anthill
    auto cells = get_cells_in_range(anthill_.x_, anthill_.y_, 1.0f);
    for (Cell* cell_p : cells) {
        cell_p->pheromone_.set(PheromoneType::Home, 1.0f);
    }
}

Cell& Simulation::get_cell(int x, int y) {
    int safe_x = (x % width_ + width_) % width_;
    int safe_y = (y % height_ + height_) % height_;

    return grid_[width_ * safe_y + safe_x];
}

const Cell& Simulation::get_cell(int x, int y) const {
    int safe_x = (x % width_ + width_) % width_;
    int safe_y = (y % height_ + height_) % height_;

    return grid_[width_ * safe_y + safe_x];
}

std::vector<Cell*> Simulation::get_cells_in_range(float center_x, float center_y, float range) {
    std::vector<Cell*> found_cells;

    float min_x = center_x - range;
    float max_x = center_x + range;
    float min_y = center_y - range;
    float max_y = center_y + range;

    int start_x = static_cast<int>(std::floor(min_x));
    int end_x   = static_cast<int>(std::floor(max_x));
    int start_y = static_cast<int>(std::floor(min_y));
    int end_y   = static_cast<int>(std::floor(max_y));

    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            found_cells.push_back(&get_cell(x, y));
        }
    }

    return found_cells;
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