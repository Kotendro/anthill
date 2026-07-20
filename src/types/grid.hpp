#pragma once

#include <vector>
#include <raylib.h>
#include <functional>
#include "types/cell.hpp"
#include "config_mapper.hpp"

class Grid
{
private:
    int width_;
    int height_;
    std::vector<Cell> cells_;

    std::reference_wrapper<std::mt19937> rng_;

    std::uniform_real_distribution<float> x_crd_; // Распределение от 0 до width_
    std::uniform_real_distribution<float> y_crd_; // Распределение от 0 до height_

    // Food
    std::uniform_real_distribution<float> food_value_;
    float spawn_timer_ = 0.0f;
    float spawn_food_chance_ = 0.0f;
    int total_food_ = 0;


    void spawn_food();
    void try_spawn_food(float delta_time);

    void set_pheromone_in_radius(Vector2 center, float radius, PheromoneType ptype);

public:
    Grid(int width, int height, std::mt19937& rng);

    Vector2 spawn_anthill();

    Cell& get_cell(Vector2 crd);
    const Cell& get_cell (Vector2 crd) const;

    const int get_width() const {return width_;}
    const int get_height() const {return height_;}

    void decrease_total_food() {total_food_ -= 1;}

    void update(float delta_time);
};


inline Grid::Grid(int width, int height, std::mt19937& rng):
    width_(width),
    height_(height),
    rng_(rng),
    x_crd_(0, width),
    y_crd_(0, height),
    food_value_(Config::World::MIN_VALUE_IN_FOOD, Config::World::MAX_VALUE_IN_FOOD)
{
    cells_.resize(width_*height_);
};

inline void Grid::update(float delta_time) {
    /* Обновление чисто объектов доступных Grid - Cells(Food, Anthill) */

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
}

inline Cell& Grid::get_cell(Vector2 crd) {
    int safe_x = (static_cast<int>(crd.x) % width_ + width_) % width_;
    int safe_y = (static_cast<int>(crd.y) % height_ + height_) % height_;

    return cells_[width_ * safe_y + safe_x];
}

inline const Cell& Grid::get_cell(Vector2 crd) const {
    int safe_x = (static_cast<int>(crd.x) % width_ + width_) % width_;
    int safe_y = (static_cast<int>(crd.y) % height_ + height_) % height_;

    return cells_[width_ * safe_y + safe_x];
}

inline void Grid::set_pheromone_in_radius(Vector2 center, float radius, PheromoneType ptype) {
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

inline void Grid::try_spawn_food(float delta_time) {
    if (total_food_ >= Config::World::MAX_FOOD) {
        return;
    }

    spawn_timer_ += delta_time;

    if (spawn_timer_ >= 1.0f) {
        spawn_timer_ -= 1.0f;

        spawn_food_chance_ = std::clamp(
            spawn_food_chance_ + Config::World::FOOD_SPAWN_GROWTH,
            0.0f,
            Config::World::MAX_FOOD_SPAWN_CHANCE
        );

        std::bernoulli_distribution distribution(spawn_food_chance_);
        
        if (distribution(rng_.get())) {
            spawn_food();
            total_food_++;
            spawn_food_chance_ = 0.0f;
        }
    }
}

inline void Grid::spawn_food() {
    float raw_x = x_crd_(rng_.get());
    float raw_y = y_crd_(rng_.get());

    float relative_x = raw_x - std::trunc(raw_x);
    float relative_y = raw_y - std::trunc(raw_y);

    Cell& cell = get_cell(Vector2{raw_x, raw_y});
    if (!cell.food_.has_value() && !cell.anthill_.has_value()) {
        cell.food_ = Food{
            relative_x,
            relative_y,
            food_value_(rng_.get())
        };
    }
}

inline Vector2 Grid::spawn_anthill() {
    float raw_x = x_crd_(rng_.get());
    float raw_y = y_crd_(rng_.get());

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