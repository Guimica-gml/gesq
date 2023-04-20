#ifndef GOL_H_
#define GOL_H_

#include <stdlib.h>

#define GOL_BOARD_DIM 25

typedef enum {
    STATE_DEAD = 0,
    STATE_ALIVE = 1,
} Gol_State;

typedef struct {
    Gol_State cells[GOL_BOARD_DIM][GOL_BOARD_DIM];
} Gol_Board;

int gol_alive_neighbor_count(Gol_Board *board, size_t x, size_t y);
void gol_board_advance(Gol_Board *board);
void gol_board_clear(Gol_Board *board);

#endif // GOL_H_
