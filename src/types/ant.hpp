#pragma once

#include <numbers>
#include "types/pheromone.hpp"
#include "types/grid.hpp"

#include "config.hpp"

class Ant
{
private:
    std::uniform_real_distribution<float> wander_angle_;

    float movement_speed_ = Config::Ant::MOVEMENT_SPEED;
    float rotation_speed_ = Config::Ant::ROTATION_SPEED;
    float antennae_angle_ = Config::Ant::ANTENNAE_ANGLE;
    float antennae_length_= Config::Ant::ANTENNAE_LENGTH;

    void pick_up_food();
    void drop_food();
    float get_required_food();
    void eat(float food);

    PheromoneType get_search_type() const;
    void move(float angle_relative, float wander_noise, float steer_force, float delta_time);
    void wrap_position(int width, int height);
    Vector2 get_antennae_relative(float angle_offset = 0.0f);
    float get_pheromone_angle_relative(
        Vector2 left_relative, float left_intst, Vector2 forward_relative, float forward_intst, Vector2 right_relative, float right_intst
    );

public:
    float x_, y_;
    float angle_; // В градусах
    bool has_food_=false;

    float energy_ = Config::Ant::MAX_ENERGY;
    Pheromone pheromone_out_;

    Ant(float x, float y, float angle);

    void update_position(const Grid& grid, float delta_time, std::mt19937& rng);
    void interact_with_world(Grid& grid, float delta_time);
};

inline Ant::Ant(float x, float y, float angle):
    wander_angle_(-Config::Ant::WANDER_ANGLE, Config::Ant::WANDER_ANGLE),
    x_(x),
    y_(y),
    angle_(angle)
{}

inline PheromoneType Ant::get_search_type() const {
    return has_food_ ? PheromoneType::Home : PheromoneType::Food;
}

inline void Ant::move(float angle_relative, float wander_noise, float steer_force, float delta_time) {
    float steer_blend = std::clamp(steer_force, 0.0f, 1.0f);

    angle_ += angle_relative * steer_blend * (rotation_speed_ * delta_time);
    angle_ += wander_noise * (2.0f - steer_blend) * (rotation_speed_ * delta_time);

    float radians = angle_ * (std::numbers::pi / 180.0f);
    x_ += std::cos(radians) * (movement_speed_ * delta_time);
    y_ += std::sin(radians) * (movement_speed_ * delta_time);
}

inline void Ant::wrap_position(int width, int height) {
    if (x_ < 0) x_ += width;
    if (x_ >= width) x_ -= width;
    if (y_ < 0) y_ += height;
    if (y_ >= height) y_ -= height;
}

inline void Ant::pick_up_food() {
    has_food_ = true;
    angle_ += 180.0f; 
}

inline void Ant::drop_food() {
    has_food_ = false;
    angle_ += 180.0f;
}

inline Vector2 Ant::get_antennae_relative(float angle_offset) {
    /* Получаем относительную координату относительно муравья, длине антенны и углу */
    float radians = (angle_ + angle_offset) * (std::numbers::pi / 180);
    float x = std::cos(radians) * antennae_length_;
    float y = std::sin(radians) * antennae_length_;
    return Vector2{x, y};
}

inline float Ant::get_pheromone_angle_relative(
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

inline float Ant::get_required_food() {
    /* Необходимое количество энергии для полного насыщения */
    return (Config::Ant::MAX_ENERGY - energy_);
}

inline void Ant::eat(float food) {
    energy_ += std::clamp(food, 0.0f, Config::Ant::MAX_ENERGY);
}

inline void Ant::update_position(const Grid& grid, float delta_time, std::mt19937& rng) {

    PheromoneType search_type = get_search_type();

    Vector2 left_relative = get_antennae_relative(-antennae_angle_);
    Vector2 forward_relative = get_antennae_relative(0.0f);
    Vector2 right_relative = get_antennae_relative(antennae_angle_);

    float left_intst = grid.get_cell(Vector2{x_ + left_relative.x, y_ + left_relative.y}).pheromone_.get(search_type);
    float forward_intst = grid.get_cell(Vector2{x_ + forward_relative.x, y_ + forward_relative.y}).pheromone_.get(search_type);
    float right_intst = grid.get_cell(Vector2{x_ + right_relative.x, y_ + right_relative.y}).pheromone_.get(search_type);

    float angle_relative = get_pheromone_angle_relative(
        left_relative, left_intst, forward_relative, forward_intst, right_relative, right_intst
    );
    
    float wander_noise = wander_angle_(rng); 
    float steer_force = left_intst + forward_intst + right_intst;
    move(angle_relative, wander_noise, steer_force, delta_time);

    wrap_position(grid.get_width(), grid.get_height());
}

inline void Ant::interact_with_world(Grid& grid, float delta_time) {
    /* Обновление, связанное с миром */

    Cell& cell = grid.get_cell(Vector2{x_, y_});

    // Ant - Anthill 
    if (cell.anthill_.has_value()) {
        Anthill& anthill = cell.anthill_.value();

        float intensity = cell.pheromone_.get(PheromoneType::Home);
        pheromone_out_.set(PheromoneType::Home, intensity);

        if (has_food_) {
            anthill.food_storage_ += 1.0f;
            drop_food();
        }

        float food = anthill.get_food(get_required_food());
        eat(food);
    }

    // Ant - Food
    if (cell.food_.has_value()) {
        float intensity = cell.pheromone_.get(PheromoneType::Food);
        pheromone_out_.set(PheromoneType::Food, intensity);

        if (!has_food_) {
            Food& food = cell.food_.value();
            food.value_ -= std::min(1.0f, food.value_);
            pick_up_food();
            if (food.value_ <= 0.0f) {
                cell.food_.reset();
                grid.decrease_total_food();
            }
        }
    }

    // Ant - Pheromone
    if (has_food_) {
        float intensity = pheromone_out_.get(PheromoneType::Food);
        cell.pheromone_.add(delta_time, PheromoneType::Food, intensity, Config::Pheromone::ORDINARY_LIMIT);
    } else {
        float intensity = pheromone_out_.get(PheromoneType::Home);
        cell.pheromone_.add(delta_time, PheromoneType::Home, intensity, Config::Pheromone::ORDINARY_LIMIT);
    }
}