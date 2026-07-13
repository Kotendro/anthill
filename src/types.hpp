#pragma once

#include <array>

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
    
    void evaporate(float delta) {
        for (auto& intensity : intensities_) {
            intensity = std::max(0.0f, intensity - delta);
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
    float x_, y_;
    int food_storage_ = 0;
    // Pheromone pheromone_out_;

    Anthill(float x, float y)
    : x_(x), y_(y) {}
};


struct Cell {
    Pheromone pheromone_;
    int food_units_ = 0;
};