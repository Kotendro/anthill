#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>

#include "config_mapper.hpp"

Simulation::Simulation(sol::state& lua) :
    rng_(std::random_device{}()),
    grid_(Config::World::WIDTH_IN_CELLS, Config::World::HEIGHT_IN_CELLS, rng_),
    start_angle_(0.0f, 360.0f),
    lua_(lua)
{
    ants_.reserve(Config::World::ANTS_COUNT);
}

void Simulation::init() {
    // Grid
    Vector2 crd = grid_.spawn_anthill();

    // Ants
    for (int i=0; i < Config::World::ANTS_COUNT; i++) {
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

        ant.interact_with_world(grid_, delta_time);
        ant.update_position(grid_, delta_time, rng_);

        ant.pheromone_out_.evaporate(delta_time, Config::Pheromone::ANT_DECAY_RATE);
    }

    std::erase_if(ants_, [](const Ant& ant) {
        return ant.energy_ <= 0.0f;
    });
}

void Simulation::reset() {
    // Grid
    grid_ = Grid(Config::World::WIDTH_IN_CELLS, Config::World::HEIGHT_IN_CELLS, rng_);

    // Ants
    ants_.clear();
    ants_.reserve(Config::World::ANTS_COUNT);

    Vector2 crd = grid_.spawn_anthill();

    for (int i=0; i < Config::World::ANTS_COUNT; i++) {
        Ant ant{crd.x, crd.y, start_angle_(rng_)};
        ants_.push_back(ant);
    }
}