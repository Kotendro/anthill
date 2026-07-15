#include "render.hpp"
#include "raylib.h"
#include "types.hpp"
#include <array>

const std::array<Color, static_cast<size_t>(PheromoneType::Count)> PHEROMONE_COLORS = {
    GREEN,   // Food
    BLUE,  // Home
};

void draw_simulation(const Simulation& sim, int cell_size)
{
    int width = sim.get_width();
    int height = sim.get_height();

    // Cells
    for (float y=0.0f; y<height; y+=1.0f) {
        for (float x=0.0f; x<width; x+=1.0f) {
            const Cell& cell = sim.get_cell(Vector2{x,y});

            DrawRectangle(x*cell_size, y*cell_size, cell_size-1, cell_size-1, LIGHTGRAY);

            /* 
            Тупая реализация через прозрачности, которая потребляет много ресурсов, зато мне очень понятна.
            Стоит потом переделать.
            */
            for (int i=0; i<cell.pheromone_.intensities_.size(); i+=1) {
                float intensity = cell.pheromone_.intensities_[i];
                unsigned char alpha = static_cast<unsigned char>(200 * intensity);
                if (alpha > 0) {
                    Color color {
                        PHEROMONE_COLORS[i].r,
                        PHEROMONE_COLORS[i].g,
                        PHEROMONE_COLORS[i].b,
                        alpha,
                    };
                    DrawRectangle(x*cell_size, y*cell_size, cell_size-1, cell_size-1, color);
                }
            }
        }
    }

    for (float y = 0.0f; y < height; y+=1.0f) {
        for (float x = 0.0f; x < width; x+=1.0f) {
            const Cell& cell = sim.get_cell(Vector2{x,y});

            // Food
            if (cell.food_.has_value()) {
                const Food& food = cell.food_.value();
                float pixel_x = (x + food.relative_x_) * cell_size;
                float pixel_y = (y + food.relative_y_) * cell_size;
                DrawCircle(pixel_x, pixel_y, cell_size / 2.0f, ORANGE);
            }

            // Anthill
            if (cell.anthill_.has_value()) {
                const Anthill& anthill = cell.anthill_.value();
                float pixel_x = (x + anthill.relative_x_) * cell_size;
                float pixel_y = (y + anthill.relative_y_) * cell_size;
                DrawCircle(pixel_x, pixel_y, cell_size / 2.0f, RED);
            }
        }
    }

    // Ants
    for (auto& ant : sim.get_ants()) {
        float pixel_x = ant.x_ * cell_size;
        float pixel_y = ant.y_ * cell_size;

        DrawCircle(pixel_x,pixel_y, cell_size/4, RED);
        if (ant.has_food) {
            DrawCircle(pixel_x,pixel_y-cell_size/4, cell_size/5, ORANGE);
        }
    }
}