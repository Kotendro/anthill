#pragma once

#include <array>
#include <cmath>

enum class PheromoneType {
    Food,
    Home,
    Count
};

struct Pheromone {
    // intensities_ = 0.0f - 1.0f
    std::array<float, static_cast<size_t>(PheromoneType::Count)> intensities_;

    Pheromone() {
        intensities_.fill(0.0f);
    }

    float get(PheromoneType type) const {
        return intensities_[static_cast<size_t>(type)];
    }

    void set(PheromoneType type, float value) {
        intensities_[static_cast<size_t>(type)] = std::clamp(value, 0.0f, 1.0f);
    }

    void add(PheromoneType type, float value, float floor) {
        size_t idx = static_cast<size_t>(type);
        float curr = intensities_[idx];

        if (curr < floor) {
            intensities_[idx] = std::min(curr + value, floor);
        }
    }

    void evaporate(float delta_time) {
        for (auto& intensity : intensities_) {
            float delta = delta_time * (intensity * intensity);
            intensity -= delta;
            if (intensity < 0.001f) {
                intensity = 0.0f;
            }
        }
    }
};

struct Ant
{
    float x_, y_;
    float angle_; // В градусах
    float speed_=10.0f;
    bool has_food=false;
    // Pheromone pheromone_out_;

    Ant(float x, float y, float angle)
    : x_(x), y_(y), angle_(angle) {}
};

struct Anthill
{
    float relative_x_, relative_y_;
    int food_storage_ = 0;
    // Pheromone pheromone_out_;

    Anthill(float relative_x, float relative_y)
    : relative_x_(relative_x), relative_y_(relative_y) {}
};

struct Food {
    float relative_x_, relative_y_;
    int count_ = 0;
    // Pheromone pheromone_out_;

    Food(float relative_x, float relative_y, int count)
    : relative_x_(relative_x), relative_y_(relative_y), count_(count) {}
};

struct Cell {
    Pheromone pheromone_;

    std::optional<Food> food_;
    std::optional<Anthill> anthill_;
};