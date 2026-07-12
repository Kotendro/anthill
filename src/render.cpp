#include "render.hpp"
#include "raylib.h"
#include "types.hpp"

void draw_simulation(const Simulation& sim, int cell_size)
{
    // Grid
    for (int y=0; y<sim.get_height(); y+=1) 
    {
        for (int x=0; x<sim.get_width(); x+=1)
        {
            const Cell& cell = sim.get_cell(x, y);
            DrawRectangle(x*cell_size, y*cell_size, cell_size-1, cell_size-1, LIGHTGRAY);
        }
    }

    // Ants
    for (auto& ant : sim.get_ants())
    {
        float pixel_x = ant.x_ * cell_size + cell_size/2;
        float pixel_y = ant.y_ * cell_size + cell_size/2;

        DrawCircle(pixel_x,pixel_y, cell_size/4, RED);
    }
}