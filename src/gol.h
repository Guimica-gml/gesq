#ifndef GOL_H_
#define GOL_H_

#include <stdlib.h>

typedef enum {
    STATE_DEAD = 0,
    STATE_ALIVE = 1,
} Gol_State;

typedef struct {
    size_t width;
    size_t height;
    Gol_State *cells;
} Gol_Board;

Gol_Board gol_board_new(size_t width, size_t height);
int gol_board_alive_neighbor_count(Gol_Board *board, size_t x, size_t y);
void gol_board_advance(Gol_Board *board);
void gol_board_copy(Gol_Board *dst, size_t x, size_t y, Gol_Board *src);
void gol_board_clear(Gol_Board *board);
void gol_board_free(Gol_Board *board);

#endif // GOL_H_
