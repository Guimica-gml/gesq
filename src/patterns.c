#include "./gol.h"

Gol_State pattern_default_cells[] = {
    1,
};

Gol_State pattern_glider_cells[] = {
    0, 1, 0,
    0, 0, 1,
    1, 1, 1,
};

Gol_State pattern_hammerhead_cells[] = {
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Gol_State pattern_cloverleaf_cells[] = {
    0, 0, 0, 1, 0, 1, 0, 0, 0,
    0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 1, 0, 1,
    0, 1, 1, 0, 1, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 1, 0, 1, 1, 0,
    1, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 1, 1, 1, 0, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 1, 0, 0, 0,
};

Gol_State pattern_light_spaceship_cells[] = {
    0, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
    1, 0, 0, 1, 0,
};

Gol_State pattern_middle_spaceship_cells[] = {
    0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1,
};

Gol_State pattern_heavy_spaceship_cells[] = {
    0, 0, 1, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1,
};

const Gol_Board pattern_default = { 1, 1, pattern_default_cells };
const Gol_Board pattern_glider = { 3, 3, pattern_glider_cells };
const Gol_Board pattern_hammerhead = { 18, 16, pattern_hammerhead_cells };
const Gol_Board pattern_cloverleaf = { 9, 11, pattern_cloverleaf_cells };
const Gol_Board pattern_light_spaceship = { 5, 4, pattern_light_spaceship_cells };
const Gol_Board pattern_middle_spaceship = { 6, 5, pattern_middle_spaceship_cells };
const Gol_Board pattern_heavy_spaceship = { 7, 5, pattern_heavy_spaceship_cells };
