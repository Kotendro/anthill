#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>
#include <iostream>



Simulation::Simulation(int width, int height, int ants_count) 
: width_(width), height_(height), ants_count_(ants_count), rng_(std::random_device{}()), angle_(-90.0f, 90.0f),
x_coord_(0, width), y_coord_(0, height)
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
    // Anthill
    spawn_anthill();

    // Ants
    for (int i=0; i < ants_count_; i++) {
        Ant ant{x_coord_(rng_), y_coord_(rng_), 0};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    // Cells
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            Cell& cell = get_cell(x, y);
            cell.pheromone_.evaporate(delta_time);

            if (cell.food_.has_value()) {
                Food& food = cell.food_.value();
                float pixel_x = x + food.relative_x_;
                float pixel_y = y + food.relative_y_;
                set_pheromone_in_radius(pixel_x, pixel_y, 2.5f, PheromoneType::Food);
            }
            if (cell.anthill_.has_value()) {
                Anthill& anthil = cell.anthill_.value();
                float pixel_x = x + anthil.relative_x_;
                float pixel_y = y + anthil.relative_y_;
                set_pheromone_in_radius(pixel_x, pixel_y, 3.0f, PheromoneType::Home);
            }
        }
    }

    // Food
    try_spawn_food();

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
        cell.pheromone_.add(PheromoneType::Home, 0.7f, 0.7f);

        if (!ant.has_food && cell.food_.has_value()) {
            Food& food = cell.food_.value();
            food.count_ -= 1;
            total_food_ -= 1;
            if (food.count_ == 0) {
                cell.food_.reset();
            }
            ant.has_food = true;
        }
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

void Simulation::set_pheromone_in_radius(float center_x, float center_y, float radius, PheromoneType ptype) {
    float min_x = center_x - radius;
    float max_x = center_x + radius;
    float min_y = center_y - radius;
    float max_y = center_y + radius;

    int start_x = static_cast<int>(std::floor(min_x));
    int end_x   = static_cast<int>(std::floor(max_x));
    int start_y = static_cast<int>(std::floor(min_y));
    int end_y   = static_cast<int>(std::floor(max_y));

    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            float cell_center_x = x + 0.5f;
            float cell_center_y = y + 0.5f;

            float dx = cell_center_x - center_x;
            float dy = cell_center_y - center_y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (radius > distance) {
                float norma = distance/radius;
                float intensity = 1.0f - (norma*norma);
                Cell& cell = get_cell(x, y);
                cell.pheromone_.add(ptype, intensity, intensity);
            }
        }
    }
}

void Simulation::try_spawn_food() {
    std::bernoulli_distribution distribution(spawn_food_chance_);
    spawn_food_chance_ = std::clamp(spawn_food_chance_+0.00001f, 0.0f, 0.1f);

    if (distribution(rng_) && total_food_<MAX_FOOD_) {
        spawn_food();
        total_food_++;
    }
}

void Simulation::spawn_food() {
    float raw_x = x_coord_(rng_);
    float raw_y = y_coord_(rng_);

    int cell_x = static_cast<int>(raw_x);
    int cell_y = static_cast<int>(raw_y);

    float relative_x = raw_x - static_cast<float>(cell_x);
    float relative_y = raw_y - static_cast<float>(cell_y);

    Cell& cell = get_cell(cell_x, cell_y);
    if (!cell.food_.has_value() && !cell.anthill_.has_value()) {
        cell.food_ = Food{
            relative_x,
            relative_y,
            1
        };
    }
}

void Simulation::spawn_anthill() {
    float raw_x = x_coord_(rng_);
    float raw_y = y_coord_(rng_);

    int cell_x = static_cast<int>(raw_x);
    int cell_y = static_cast<int>(raw_y);

    float relative_x = raw_x - static_cast<float>(cell_x);
    float relative_y = raw_y - static_cast<float>(cell_y);

    Cell& cell = get_cell(cell_x, cell_y);
    if (!cell.anthill_.has_value() && !cell.anthill_.has_value()) {
        cell.anthill_ = Anthill{
            relative_x,
            relative_y,
        };
    }
}