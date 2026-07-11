#include "render.hpp"
#include "raylib.h"

void draw_simulation(const Simulation& sim, int cell_size)
{
    // Grid
    for (int h=0; h<sim.get_height(); h+=1) 
    {
        for (int w=0; w<sim.get_width(); w+=1)
        {
            DrawRectangle(w*cell_size, h*cell_size, cell_size-1, cell_size-1, LIGHTGRAY);
        }
    }

    // Ants
    for (auto& ant : sim.get_ants())
    {
        float pixel_x = ant.x_ * (cell_size-1) + cell_size/2;
        float pixel_y = ant.y_ * (cell_size-1) + cell_size/2;

        DrawCircle(pixel_x,pixel_y, cell_size/4, RED);
    }
}