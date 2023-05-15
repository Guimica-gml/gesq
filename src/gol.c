#include <string.h>
#include "./gol.h"

Gol_Board gol_board_new(size_t width, size_t height) {
    return (Gol_Board) {
        .cells = malloc(width * height * sizeof(Gol_State)),
        .width = width,
        .height = height,
    };
}

int gol_alive_neighbor_count(Gol_Board *board, size_t x, size_t y) {
    int neighbor_count = 0;

    for (int dy = -1; dy < 2; ++dy) {
        for (int dx = -1; dx < 2; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = (x + dx + board->width) % board->width;
            int ny = (y + dy + board->height) % board->height;
            neighbor_count += board->cells[ny * board->width + nx];
        }
    }

    return neighbor_count;
}

void gol_board_advance(Gol_Board *board) {
    Gol_Board board_copy = { 0 };
    board_copy.width = board->width;
    board_copy.height = board->height;
    board_copy.cells = malloc(board->width * board->height * sizeof(Gol_State));
    memcpy(board_copy.cells, board->cells, board->width * board->height * sizeof(Gol_State));

    for (size_t y = 0; y < board->height; ++y) {
        for (size_t x = 0; x < board->width; ++x) {
            int neighbor_count = gol_alive_neighbor_count(&board_copy, x, y);
            int index = y * board->width + x;

            switch (board->cells[index]) {
                case STATE_DEAD: {
                    if (neighbor_count == 3) {
                        board->cells[index] = STATE_ALIVE;
                    }
                } break;
                case STATE_ALIVE: {
                    if (neighbor_count != 2 && neighbor_count != 3) {
                        board->cells[index] = STATE_DEAD;
                    }
                } break;
            }
        }
    }

    free(board_copy.cells);
}

void gol_board_clear(Gol_Board *board) {
    memset(board->cells, STATE_DEAD, board->width * board->height * sizeof(Gol_State));
}

void gol_board_free(Gol_Board *board) {
    free(board->cells);
}

void gol_pattern_switch(Gol_Pattern *pattern, Gol_State *cells, size_t width, size_t height) {
    pattern->width = width;
    pattern->height = height;
    pattern->cells = realloc(pattern->cells, width * height * sizeof(Gol_State));
    memcpy(pattern->cells, cells, width * height * sizeof(Gol_State));
}
