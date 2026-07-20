#include <ctime>
#include <cstdlib>
#include <sol/sol.hpp>
#include <raylib.h>
#include <iostream>

#include "simulation.hpp"
#include "render.hpp"

#include "config_mapper.hpp"


int main(void)
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);
    try {
        lua.script_file("scripts/config.lua");
        Config::load_from_lua(lua);
    } catch (const sol::error& e) {
        std::cerr << "Lua config script error" << std::endl;
        return -1;
    }

    Simulation sim(lua);
    sim.init();

    InitWindow(
        Config::World::SCREEN_WIDTH,
        Config::World::SCREEN_HEIGHT,
        "anthill"
    );
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R)) {
            try {
                lua.script_file("scripts/config.lua");
                Config::load_from_lua(lua);
                std::cout << "Lua config reloaded" << std::endl;
            } catch (const sol::error& e) {
                std::cerr << "Lua config script error" << std::endl;
                return -1;
            }
            sim.reset();
            SetWindowSize(Config::World::SCREEN_WIDTH, Config::World::SCREEN_HEIGHT);
        }

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