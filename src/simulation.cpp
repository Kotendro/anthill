#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist(-20, 20);

Simulation::Simulation(int width, int height, int ants_count) 
: width_(width), height_(height), ants_count_(ants_count)
{
    grid_.reserve(width_*height_);
    ants_.reserve(ants_count_);
}

void Simulation::init() {
    for (int i=0; i < ants_count_; i++) {
        Ant ant{0, 0, 0};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    for (auto &ant : ants_) {
        ant.angle_ += dist(rng) * delta_time;
        
        float radians = ant.angle_ * (std::numbers::pi / 180);
        ant.x_ += std::cos(radians) * ant.speed_ * delta_time;
        ant.y_ += std::sin(radians) * ant.speed_ * delta_time;

        if (ant.x_ < 0) ant.x_ = width_ - 1;
        if (ant.x_ >= width_) ant.x_ = 0;
        if (ant.y_ < 0) ant.y_ = height_ - 1;
        if (ant.y_ >= height_) ant.y_ = 0;
    }
}