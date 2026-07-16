#pragma once

#include <cstddef>

/*
Пояснения:
    сек. - секунды
    гр. - градусы поворота
    кл. - клеток
*/

namespace Config {
    namespace World {
        inline constexpr int WIDTH_IN_CELLS = 80;
        inline constexpr int HEIGHT_IN_CELLS = 60;
        inline constexpr int CELL_SIZE = 20;
        inline constexpr int ANTS_COUNT = 20;
        inline constexpr int SCREEN_WIDTH = WIDTH_IN_CELLS*CELL_SIZE;
        inline constexpr int SCREEN_HEIGHT = HEIGHT_IN_CELLS*CELL_SIZE;

        inline const int MAX_FOOD = 5; // Максимальное количество еды в мире
        inline constexpr float FOOD_SPAWN_GROWTH = 0.01f; // В сек. | На сколько повышается вероятность появления еды
        inline constexpr float MAX_FOOD_SPAWN_CHANCE = 0.1f;
        inline constexpr int MIN_COUNT_IN_FOOD = 2;  // Минимальное количество раз, сколько можно взять еды с одного плода
        inline constexpr int MAX_COUNT_IN_FOOD = 10; // Максимальное количество раз, сколько можно взять еды с одного плода
    }

    namespace Ant {
        inline constexpr float MOVEMENT_SPEED = 8.0f;   // В сек.
        inline constexpr float ROTATION_SPEED = 90.0f;  // В сек. | Резкость поворота при запахе
        inline constexpr float ANTENNAE_ANGLE = 45.0f;  // В гр.  | Угол развода усиков
        inline constexpr float ANTENNAE_LENGTH = 2.0f;  // В кл.
        inline constexpr float WANDER_ANGLE = 5.0f;     // В гр.  | Резкость поворота пустом месте
        inline constexpr float ENERGY_DECAY = 0.02f;     // В сек. | Затрата энергии в секунду
        inline constexpr float MAX_ENERGY = 1.0f;
    }

    namespace Pheromone {
        inline constexpr float MAP_DECAY_RATE = 0.08f; // Испарение на карте
        inline constexpr float ANT_DECAY_RATE = 1.6f;  // Испарение запаса муравья 
        inline constexpr float ORDINARY_LIMIT = 0.8f;  // Потолок силы для обычных троп муравьев
        inline constexpr float EVAPORATION_THRESHOLD = 0.001f; // Порог, ниже которого запах зануляется
        inline constexpr float DEPOSIT_RATE = 15.0f;   // Cкорость накопления запаха на клетке в секунду
    }
}