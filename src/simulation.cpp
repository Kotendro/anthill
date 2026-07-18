#include "simulation.hpp"
#include <cstdlib> 
#include <numbers>
#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>

#include "config.hpp"

Simulation::Simulation(int width, int height, int ants_count) :
    width_(width),
    height_(height),
    ants_count_(ants_count),
    rng_(std::random_device{}()),
    wander_angle_(-Config::Ant::WANDER_ANGLE, Config::Ant::WANDER_ANGLE),
    start_angle_(0.0f, 360.0f),
    x_crd_(0, width),
    y_crd_(0, height),
    food_value_(Config::World::MIN_VALUE_IN_FOOD, Config::World::MAX_VALUE_IN_FOOD)
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
    Vector2 crd = spawn_anthill();

    // Ants
    for (int i=0; i < ants_count_; i++) {
        Ant ant{crd.x, crd.y, start_angle_(rng_)};
        ants_.push_back(ant);
    }
}

void Simulation::update(float delta_time) {
    // Cells
    for (float y = 0.0f; y < height_; y+= 1.0f) {
        for (float x = 0.0f; x < width_; x+= 1.0f) {
            Cell& cell = get_cell(Vector2{x, y});
            cell.pheromone_.evaporate(delta_time, Config::Pheromone::MAP_DECAY_RATE);

            if (cell.food_.has_value()) {
                Food& food = cell.food_.value();
                float pixel_x = x + food.relative_x_;
                float pixel_y = y + food.relative_y_;
                set_pheromone_in_radius(Vector2{pixel_x, pixel_y}, 2.5f, PheromoneType::Food);
            }
            if (cell.anthill_.has_value()) {
                Anthill& anthil = cell.anthill_.value();
                float pixel_x = x + anthil.relative_x_;
                float pixel_y = y + anthil.relative_y_;
                set_pheromone_in_radius(Vector2{pixel_x, pixel_y}, 3.0f, PheromoneType::Home);
            }
        }
    }

    // Food
    try_spawn_food(delta_time);

    // Ants
    for (Ant& ant : ants_) {
        ant.energy_ -= Config::Ant::ENERGY_DECAY * delta_time;

        PheromoneType search_type = ant.get_search_type();

        Vector2 left_relative = ant.get_antennae_relative(-ant.antennae_angle_);
        Vector2 forward_relative = ant.get_antennae_relative(0.0f);
        Vector2 right_relative = ant.get_antennae_relative(ant.antennae_angle_);

        float left_intst = get_cell(Vector2{ant.x_ + left_relative.x, ant.y_ + left_relative.y}).pheromone_.get(search_type);
        float forward_intst = get_cell(Vector2{ant.x_ + forward_relative.x, ant.y_ + forward_relative.y}).pheromone_.get(search_type);
        float right_intst = get_cell(Vector2{ant.x_ + right_relative.x, ant.y_ + right_relative.y}).pheromone_.get(search_type);

        float angle_relative = ant.get_pheromone_angle_relative(
            left_relative, left_intst, forward_relative, forward_intst, right_relative, right_intst
        );
        
        float wander_noise = wander_angle_(rng_); 
        float steer_force = left_intst + forward_intst + right_intst;
        ant.move(angle_relative, wander_noise, steer_force, delta_time);

        ant.wrap_position(width_, height_);


        Cell& cell = get_cell(Vector2{ant.x_, ant.y_});
        if (cell.anthill_.has_value()) {
            Anthill& anthill = cell.anthill_.value();

            float intensity = cell.pheromone_.get(PheromoneType::Home);
            ant.pheromone_out_.set(PheromoneType::Home, intensity);

            if (ant.has_food_) {
                anthill.food_storage_ += 1.0f;
                ant.drop_food();
            }

            float food = anthill.get_food(ant.get_required_food());
            ant.eat(food);
        }
        if (cell.food_.has_value()) {
            float intensity = cell.pheromone_.get(PheromoneType::Food);
            ant.pheromone_out_.set(PheromoneType::Food, intensity);

            if (!ant.has_food_) {
                Food& food = cell.food_.value();
                food.value_ -= std::min(1.0f, food.value_);
                ant.pick_up_food();
                if (food.value_ <= 0.0f) {
                    cell.food_.reset();
                    total_food_ -= 1;
                }
            }
        }

        if (ant.has_food_) {
            float intensity = ant.pheromone_out_.get(PheromoneType::Food);
            cell.pheromone_.add(delta_time, PheromoneType::Food, intensity, Config::Pheromone::ORDINARY_LIMIT);
        } else {
            float intensity = ant.pheromone_out_.get(PheromoneType::Home);
            cell.pheromone_.add(delta_time, PheromoneType::Home, intensity, Config::Pheromone::ORDINARY_LIMIT);
        }

        ant.pheromone_out_.evaporate(delta_time, Config::Pheromone::ANT_DECAY_RATE);
    }

    std::erase_if(ants_, [](const Ant& ant) {
        return ant.energy_ <= 0.0f;
    });
}

Cell& Simulation::get_cell(Vector2 crd) {
    int safe_x = (static_cast<int>(crd.x) % width_ + width_) % width_;
    int safe_y = (static_cast<int>(crd.y) % height_ + height_) % height_;

    return grid_[width_ * safe_y + safe_x];
}

const Cell& Simulation::get_cell(Vector2 crd) const {
    int safe_x = (static_cast<int>(crd.x) % width_ + width_) % width_;
    int safe_y = (static_cast<int>(crd.y) % height_ + height_) % height_;

    return grid_[width_ * safe_y + safe_x];
}

void Simulation::set_pheromone_in_radius(Vector2 center, float radius, PheromoneType ptype) {
    float min_x = center.x - radius;
    float max_x = center.x + radius;
    float min_y = center.y - radius;
    float max_y = center.y + radius;

    int start_x = std::floor(min_x);
    int end_x   = std::floor(max_x);
    int start_y = std::floor(min_y);
    int end_y   = std::floor(max_y);
    int center_x   = std::floor(center.x);
    int center_y   = std::floor(center.y);


    for (float y = start_y; y <= end_y; y+=1.0f) {
        for (float x = start_x; x <= end_x; x+=1.0f) {
            float cell_center_x = x + 0.5f;
            float cell_center_y = y + 0.5f;

            float dx = cell_center_x - center.x;
            float dy = cell_center_y - center.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (radius > distance) {
                float intensity;
                if (static_cast<int>(x) == center_x && static_cast<int>(y) == center_y) {
                    intensity = 1.0f;
                } else {
                    float norma = distance/radius;
                    intensity = 1.0f - (norma*norma);
                }
                Cell& cell = get_cell(Vector2{x, y});
                cell.pheromone_.static_add(ptype, intensity);
            }
        }
    }
}

void Simulation::try_spawn_food(float delta_time) {
    std::bernoulli_distribution distribution(spawn_food_chance_);
    spawn_food_chance_ = std::clamp(
        spawn_food_chance_+(Config::World::FOOD_SPAWN_GROWTH * delta_time),
        0.0f,
        Config::World::MAX_FOOD_SPAWN_CHANCE
    );

    if (distribution(rng_) && total_food_<Config::World::MAX_FOOD) {
        spawn_food();
        total_food_++;
    }
}

void Simulation::spawn_food() {
    float raw_x = x_crd_(rng_);
    float raw_y = y_crd_(rng_);

    float relative_x = raw_x - std::trunc(raw_x);
    float relative_y = raw_y - std::trunc(raw_y);

    Cell& cell = get_cell(Vector2{raw_x, raw_y});
    if (!cell.food_.has_value() && !cell.anthill_.has_value()) {
        cell.food_ = Food{
            relative_x,
            relative_y,
            food_value_(rng_)
        };
    }
}

Vector2 Simulation::spawn_anthill() {
    float raw_x = x_crd_(rng_);
    float raw_y = y_crd_(rng_);

    float relative_x = raw_x - std::trunc(raw_x);
    float relative_y = raw_y - std::trunc(raw_y);

    Cell& cell = get_cell(Vector2{raw_x, raw_y});
    if (!cell.anthill_.has_value() && !cell.anthill_.has_value()) {
        cell.anthill_ = Anthill{
            relative_x,
            relative_y,
        };
    }

    return Vector2 {raw_x, raw_y};
}