#include "simulation.hpp"
#include "render.hpp"
#include "raylib.h"
#include <ctime>
#include <cstdlib>
#include <sol/sol.hpp> 

int main(void)
{
    const int WIDTH = 80;
    const int HEIGHT = 60;
    const int CELL_SIZE = 10;
    const int SCREEN_WIDTH = WIDTH*CELL_SIZE;
    const int SCREEN_HEIGHT = HEIGHT*CELL_SIZE;
    const int ANTS_COUNT = 20;


    Simulation sim(WIDTH, HEIGHT, ANTS_COUNT);
    sim.init();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,"anthill");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();
        sim.update(delta_time);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            draw_simulation(sim, CELL_SIZE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}