#include <ctime>
#include <cstdlib>
#include <sol/sol.hpp>
#include <raylib.h>

#include "simulation.hpp"
#include "render.hpp"

#include "config.hpp"


int main(void)
{
    Simulation sim(
        Config::World::WIDTH_IN_CELLS,
        Config::World::HEIGHT_IN_CELLS,
        Config::World::ANTS_COUNT
    );
    sim.init();

    InitWindow(
        Config::World::SCREEN_WIDTH,
        Config::World::SCREEN_HEIGHT,
        "anthill"
    );
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();
        sim.update(delta_time);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            draw_simulation(sim, Config::World::CELL_SIZE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}