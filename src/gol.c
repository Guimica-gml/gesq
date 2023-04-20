#include <string.h>
#include "./gol.h"

int gol_alive_neighbor_count(Gol_Board *board, size_t x, size_t y) {
    int neighbor_count = 0;

    for (int dy = -1; dy < 2; ++dy) {
        for (int dx = -1; dx < 2; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = (x + dx + GOL_BOARD_DIM) % GOL_BOARD_DIM;
            int ny = (y + dy + GOL_BOARD_DIM) % GOL_BOARD_DIM;
            neighbor_count += board->cells[ny][nx];
        }
    }

    return neighbor_count;
}

void gol_board_advance(Gol_Board *board) {
    Gol_Board *board_copy = malloc(sizeof(Gol_Board));
    memcpy(board_copy, board, sizeof(Gol_Board));

    for (size_t y = 0; y < GOL_BOARD_DIM; ++y) {
        for (size_t x = 0; x < GOL_BOARD_DIM; ++x) {
            int neighbor_count = gol_alive_neighbor_count(board_copy, x, y);
            switch (board->cells[y][x]) {
                case STATE_DEAD: {
                    if (neighbor_count == 3) {
                        board->cells[y][x] = STATE_ALIVE;
                    }
                } break;
                case STATE_ALIVE: {
                    if (neighbor_count != 2 && neighbor_count != 3) {
                        board->cells[y][x] = STATE_DEAD;
                    }
                } break;
            }
        }
    }

    free(board_copy);
}

void gol_board_clear(Gol_Board *board) {
    memset(board, STATE_DEAD, sizeof(Gol_Board));
}
