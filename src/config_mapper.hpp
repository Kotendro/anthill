#pragma once

#include <cstddef>

/*
Динамически обновляется из lua конфига.

Пояснения:
    сек. - секунды
    гр. - градусы поворота
    кл. - клеток
*/

namespace Config {
    namespace World {
        inline int WIDTH_IN_CELLS;
        inline int HEIGHT_IN_CELLS;
        inline int CELL_SIZE;
        inline int ANTS_COUNT;
        inline int SCREEN_WIDTH;
        inline int SCREEN_HEIGHT;

        inline int MAX_FOOD; // Максимальное количество еды в мире
        inline float FOOD_SPAWN_GROWTH; // В сек. | На сколько повышается вероятность появления еды
        inline float MAX_FOOD_SPAWN_CHANCE;
        inline int MIN_VALUE_IN_FOOD;  // Минимальное количество раз, сколько можно взять еды с одного плода
        inline int MAX_VALUE_IN_FOOD; // Максимальное количество раз, сколько можно взять еды с одного плода
    }

    namespace Ant {
        inline float MOVEMENT_SPEED;   // В сек.
        inline float ROTATION_SPEED;  // В сек. | Резкость поворота при запахе
        inline float ANTENNAE_ANGLE;  // В гр.  | Угол развода усиков
        inline float ANTENNAE_LENGTH;  // В кл.
        inline float WANDER_ANGLE;     // В гр.  | Резкость поворота пустом месте
        inline float ENERGY_DECAY;    // В сек. | Затрата энергии в секунду
        inline float MAX_ENERGY;
    }

    namespace Pheromone {
        inline float MAP_DECAY_RATE; // Испарение на карте
        inline float ANT_DECAY_RATE;  // Испарение запаса муравья 
        inline float ORDINARY_LIMIT;  // Потолок силы для обычных троп муравьев
        inline float EVAPORATION_THRESHOLD; // Порог, ниже которого запах зануляется
        inline float DEPOSIT_RATE;   // Cкорость накопления запаха на клетке в секунду
    }

    inline void load_from_lua(const sol::state& lua) {
        // World
        World::WIDTH_IN_CELLS        = lua["Config"]["World"]["WIDTH_IN_CELLS"];
        World::HEIGHT_IN_CELLS       = lua["Config"]["World"]["HEIGHT_IN_CELLS"];
        World::CELL_SIZE             = lua["Config"]["World"]["CELL_SIZE"];
        World::ANTS_COUNT            = lua["Config"]["World"]["ANTS_COUNT"];
        World::SCREEN_WIDTH          = lua["Config"]["World"]["SCREEN_WIDTH"];
        World::SCREEN_HEIGHT         = lua["Config"]["World"]["SCREEN_HEIGHT"];
        
        World::MAX_FOOD              = lua["Config"]["World"]["MAX_FOOD"];
        World::FOOD_SPAWN_GROWTH     = lua["Config"]["World"]["FOOD_SPAWN_GROWTH"];
        World::MAX_FOOD_SPAWN_CHANCE = lua["Config"]["World"]["MAX_FOOD_SPAWN_CHANCE"];
        World::MIN_VALUE_IN_FOOD     = lua["Config"]["World"]["MIN_VALUE_IN_FOOD"];
        World::MAX_VALUE_IN_FOOD     = lua["Config"]["World"]["MAX_VALUE_IN_FOOD"];

        // Ant
        Ant::MOVEMENT_SPEED          = lua["Config"]["Ant"]["MOVEMENT_SPEED"];
        Ant::ROTATION_SPEED          = lua["Config"]["Ant"]["ROTATION_SPEED"];
        Ant::ANTENNAE_ANGLE          = lua["Config"]["Ant"]["ANTENNAE_ANGLE"];
        Ant::ANTENNAE_LENGTH         = lua["Config"]["Ant"]["ANTENNAE_LENGTH"];
        Ant::WANDER_ANGLE            = lua["Config"]["Ant"]["WANDER_ANGLE"];
        Ant::ENERGY_DECAY            = lua["Config"]["Ant"]["ENERGY_DECAY"];
        Ant::MAX_ENERGY              = lua["Config"]["Ant"]["MAX_ENERGY"];

        // Pheromone
        Pheromone::MAP_DECAY_RATE        = lua["Config"]["Pheromone"]["MAP_DECAY_RATE"];
        Pheromone::ANT_DECAY_RATE        = lua["Config"]["Pheromone"]["ANT_DECAY_RATE"];
        Pheromone::ORDINARY_LIMIT        = lua["Config"]["Pheromone"]["ORDINARY_LIMIT"];
        Pheromone::EVAPORATION_THRESHOLD = lua["Config"]["Pheromone"]["EVAPORATION_THRESHOLD"];
        Pheromone::DEPOSIT_RATE          = lua["Config"]["Pheromone"]["DEPOSIT_RATE"];
    }
    
}