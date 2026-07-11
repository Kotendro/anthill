#pragma once

#include <vector>

struct Ant
{
    float x_, y_;
    float angle_;
    float speed_=30.0f;
    Ant(float x, float y, float angle)
    : x_(x), y_(y), angle_(angle) {}
};

struct Cell {

};

class Simulation
{
private:
    int width_;
    int height_;
    int ants_count_;
    std::vector<Cell> grid_;
    std::vector<Ant> ants_;

public:
    const int get_width() const {return width_;}
    const int get_height() const {return height_;}
    const std::vector<Cell>& get_grid() const {return grid_;}
    const std::vector<Ant>& get_ants() const {return ants_;}

    Simulation(int width, int height, int ant_count);
    void init();
    void update(float delta_time);
};
