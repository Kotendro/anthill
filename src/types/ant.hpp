#pragma once

#include <numbers>
#include "types/pheromone.hpp"

#include "config.hpp"

class Ant
{
public:
    float x_, y_;
    float angle_; // В градусах
    bool has_food_=false;
    Pheromone pheromone_out_;

    const float movement_speed_ = Config::Ant::MOVEMENT_SPEED;
    const float rotation_speed_ = Config::Ant::ROTATION_SPEED;
    const float antennae_angle_ = Config::Ant::ANTENNAE_ANGLE;
    const float antennae_length_= Config::Ant::ANTENNAE_LENGTH;

    Ant(float x, float y, float angle)
    : x_(x), y_(y), angle_(angle) {}

    PheromoneType get_search_type() const {
        return has_food_ ? PheromoneType::Home : PheromoneType::Food;
    }

    void move(float angle_relative, float wander_noise, float steer_force, float delta_time) {
        float steer_blend = std::clamp(steer_force, 0.0f, 1.0f);

        angle_ += angle_relative * steer_blend * (rotation_speed_ * delta_time);
        angle_ += wander_noise * (2.0f - steer_blend) * (rotation_speed_ * delta_time);

        float radians = angle_ * (std::numbers::pi / 180.0f);
        x_ += std::cos(radians) * (movement_speed_ * delta_time);
        y_ += std::sin(radians) * (movement_speed_ * delta_time);
    }

    void wrap_position(int width, int height) {
        if (x_ < 0) x_ += width;
        if (x_ >= width) x_ -= width;
        if (y_ < 0) y_ += height;
        if (y_ >= height) y_ -= height;
    }

    void pick_up_food() {
        has_food_ = true;
        angle_ += 180.0f; 
    }

    void drop_food() {
        has_food_ = false;
        angle_ += 180.0f;
    }

    Vector2 get_antennae_relative(float angle_offset = 0.0f) {
        /* Получаем координату относительно муравья по длине антенны и углу */
        float radians = (angle_ + angle_offset) * (std::numbers::pi / 180);
        float x = std::cos(radians) * antennae_length_;
        float y = std::sin(radians) * antennae_length_;
        return Vector2{x, y};
    }

    float get_pheromone_angle_relative(
        Vector2 left_relative, float left_intst, Vector2 forward_relative, float forward_intst, Vector2 right_relative, float right_intst
    ) {
        /* По интенсивности 3-х датчиков определяет на сколько градусов от текущего поворота лучше двинуться */
        float x = left_relative.x * left_intst + forward_relative.x * forward_intst + right_relative.x * right_intst;
        float y = left_relative.y * left_intst + forward_relative.y * forward_intst + right_relative.y * right_intst;
        float target_angle = std::atan2(y, x) * (180.0f / std::numbers::pi);
        float relative_angle = target_angle - angle_;

        while (relative_angle > 180.0f)  relative_angle -= 360.0f;
        while (relative_angle < -180.0f) relative_angle += 360.0f;

        return relative_angle;
    }
};
