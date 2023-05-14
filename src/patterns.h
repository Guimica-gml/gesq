#include "./gol.h"

#define PAT_DEFAULT_W 1
#define PAT_DEFAULT_H 1
Gol_State pattern_default[PAT_DEFAULT_H * PAT_DEFAULT_W] = {
    1,
};

#define PAT_GLIDER_W 3
#define PAT_GLIDER_H 3
Gol_State pattern_glider[PAT_GLIDER_H * PAT_GLIDER_W] = {
    0, 1, 0,
    0, 0, 1,
    1, 1, 1,
};

#define PAT_HAMMERHEAD_W 18
#define PAT_HAMMERHEAD_H 16
Gol_State pattern_hammerhead[PAT_HAMMERHEAD_H * PAT_HAMMERHEAD_W] = {
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

#define PAT_CLOVERLEAF_W 9
#define PAT_CLOVERLEAF_H 11
Gol_State pattern_cloverleaf[PAT_CLOVERLEAF_W * PAT_CLOVERLEAF_H] = {
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

#define PAT_LIGHT_SPACESHIP_W 5
#define PAT_LIGHT_SPACESHIP_H 4
Gol_State pattern_light_spaceship[PAT_LIGHT_SPACESHIP_W * PAT_LIGHT_SPACESHIP_H] = {
    0, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
    1, 0, 0, 1, 0,
};

#define PAT_MIDDLE_SPACESHIP_W 6
#define PAT_MIDDLE_SPACESHIP_H 5
Gol_State pattern_middle_spaceship[PAT_MIDDLE_SPACESHIP_W * PAT_MIDDLE_SPACESHIP_H] = {
    0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1,
};

#define PAT_HEAVY_SPACESHIP_W 7
#define PAT_HEAVY_SPACESHIP_H 5
Gol_State pattern_heavy_spaceship[PAT_HEAVY_SPACESHIP_W * PAT_HEAVY_SPACESHIP_H] = {
    0, 0, 1, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1,
};
