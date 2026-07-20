#pragma once

#include <array>
#include <cmath>
#include <algorithm>

#include "config_mapper.hpp"

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

    void add(float delta_time, PheromoneType type, float value, float limit) {
        size_t idx = static_cast<size_t>(type);
        float curr = intensities_[idx];

        if (curr < limit) {
            intensities_[idx] = curr + value * (limit - curr) * Config::Pheromone::DEPOSIT_RATE * delta_time;
        }
    }

    void static_add(PheromoneType type, float value) {
        size_t idx = static_cast<size_t>(type);
        float curr = intensities_[idx];

        if (curr < value) {
            intensities_[idx] = value;
        }
    }

    void evaporate(float delta_time, float decay_rate=1.0f) {
        for (auto& intensity : intensities_) {
            float delta = delta_time * decay_rate * (intensity * intensity);
            intensity -= delta;
            if (intensity < Config::Pheromone::EVAPORATION_THRESHOLD) {
                intensity = 0.0f;
            }
        }
    }

    void reset() {
        intensities_.fill(0.0f);
    }
};