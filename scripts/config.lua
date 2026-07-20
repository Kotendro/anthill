--[[
Пояснения:
    сек. - секунды
    гр. - градусы поворота
    кл. - клеток
]]

Config = {
    World = {
        WIDTH_IN_CELLS = 40,
        HEIGHT_IN_CELLS = 30,
        CELL_SIZE = 20,
        ANTS_COUNT = 2,

        MAX_FOOD = 5, -- Максимальное количество еды в мире
        FOOD_SPAWN_GROWTH = 0.1, -- В сек. | На сколько повышается вероятность появления еды
        MAX_FOOD_SPAWN_CHANCE = 0.5,
        MIN_VALUE_IN_FOOD = 2.0,  -- Минимальное количество раз, сколько можно взять еды с одного плода
        MAX_VALUE_IN_FOOD = 10.0, -- Максимальное количество раз, сколько можно взять еды с одного плода
    },

    Ant = {
        MOVEMENT_SPEED = 8.0,   -- В сек.
        ROTATION_SPEED = 90.0,  -- В сек. | Резкость поворота при запахе
        ANTENNAE_ANGLE = 45.0,  -- В гр.  | Угол развода усиков
        ANTENNAE_LENGTH = 2.0,  -- В кл.
        WANDER_ANGLE = 5.0,     -- В гр.  | Резкость поворота пустом месте
        ENERGY_DECAY = 0.00,    -- В сек. | Затрата энергии в секунду
        MAX_ENERGY = 1.0,
    },

    Pheromone = {
        MAP_DECAY_RATE = 0.08, -- Испарение на карте
        ANT_DECAY_RATE = 1.6,  -- Испарение запаса муравья 
        ORDINARY_LIMIT = 0.8,  -- Потолок силы для обычных троп муравьев
        EVAPORATION_THRESHOLD = 0.001, -- Порог, ниже которого запах зануляется
        DEPOSIT_RATE = 15.0,   -- Cкорость накопления запаха на клетке в секунду
    }
}

-- Динамически посчитанные константы
Config.World.SCREEN_WIDTH = Config.World.WIDTH_IN_CELLS * Config.World.CELL_SIZE
Config.World.SCREEN_HEIGHT = Config.World.HEIGHT_IN_CELLS * Config.World.CELL_SIZE