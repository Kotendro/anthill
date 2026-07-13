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
    // Grid
    for (int y=0; y<sim.get_height(); y+=1) {
        for (int x=0; x<sim.get_width(); x+=1) {
            const Cell& cell = sim.get_cell(x, y);

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

    // Food
    for (auto& food : sim.get_food()) {
        float pixel_x = food.x_ * cell_size;
        float pixel_y = food.y_ * cell_size;

        DrawCircle(pixel_x,pixel_y, cell_size/4, ORANGE);
    }


    // Ants
    for (auto& ant : sim.get_ants()) {
        float pixel_x = ant.x_ * cell_size;
        float pixel_y = ant.y_ * cell_size;

        DrawCircle(pixel_x,pixel_y, cell_size/4, RED);
    }

    // Anthill
    {
        auto& anthill = sim.get_anthill();
        float pixel_x = anthill.x_ * cell_size;
        float pixel_y = anthill.y_ * cell_size;

        DrawCircle(pixel_x,pixel_y, cell_size/2, RED);
    }
}