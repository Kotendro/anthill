#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>

#include "config.hpp"

Simulation::Simulation(int width, int height, int ants_count) :
    rng_(std::random_device{}()),
    grid_(width, height, rng_),
    ants_count_(ants_count),
    start_angle_(0.0f, 360.0f)
{
    ants_.reserve(ants_count_);

    try {
        lua_.script_file("scripts/ant.lua");
    } catch (const sol::error& e) {
        std::cerr << "Lua script error" << std::endl;
    }
}

void Simulation::init() {
    // Grid
    Vector2 crd = grid_.spawn_anthill();

    // Ants
    for (int i=0; i < ants_count_; i++) {
        Ant ant{crd.x, crd.y, start_angle_(rng_)};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    // Grid
    grid_.update(delta_time);

    // Ants
    for (Ant& ant : ants_) {
        ant.energy_ -= Config::Ant::ENERGY_DECAY * delta_time;

        ant.update_position(grid_, delta_time, rng_);
        ant.interact_with_world(grid_, delta_time);

        ant.pheromone_out_.evaporate(delta_time, Config::Pheromone::ANT_DECAY_RATE);
    }

    std::erase_if(ants_, [](const Ant& ant) {
        return ant.energy_ <= 0.0f;
    });
}