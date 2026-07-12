#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>



Simulation::Simulation(int width, int height, int ants_count) 
: width_(width), height_(height), ants_count_(ants_count), rng_(std::random_device{}()), dist_(-90.0f, 90.0f)
{
    grid_.resize(width_*height_);
    ants_.reserve(ants_count_);

    lua_.script_file("scripts/ant.lua");
}

void Simulation::init() {
    for (int i=0; i < ants_count_; i++) {
        Ant ant{0, 0, 0};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    for (auto &ant : ants_) {
        ant.angle_ += dist_(rng_) * delta_time * 10.f;
        
        float radians = ant.angle_ * (std::numbers::pi / 180);
        ant.x_ += std::cos(radians) * ant.speed_ * delta_time;
        ant.y_ += std::sin(radians) * ant.speed_ * delta_time;

        if (ant.x_ < -1) ant.x_ = width_ - 1;
        if (ant.x_ >= width_) ant.x_ = 0;
        if (ant.y_ < -1) ant.y_ = height_ - 1;
        if (ant.y_ >= height_) ant.y_ = 0;
    }
}

Cell& Simulation::get_cell(int x, int y) {
    return grid_.at(width_*y + x);
}

const Cell& Simulation::get_cell(int x, int y) const {
    return grid_.at(width_*y + x);
}