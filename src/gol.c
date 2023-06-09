#include <string.h>
#include "./gol.h"

Gol_Board gol_board_new(size_t width, size_t height) {
    return (Gol_Board) {
        .cells = calloc(width * height, sizeof(Gol_State)),
        .width = width,
        .height = height,
    };
}

int gol_board_alive_neighbor_count(Gol_Board *board, size_t x, size_t y) {
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
    size_t copy_size = board->width * board->height * sizeof(Gol_State);

    Gol_Board copy = {
        .width = board->width,
        .height = board->height,
        .cells = malloc(copy_size),
    };
    memcpy(copy.cells, board->cells, copy_size);

    for (size_t y = 0; y < board->height; ++y) {
        for (size_t x = 0; x < board->width; ++x) {
            int neighbor_count = gol_board_alive_neighbor_count(&copy, x, y);
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

    free(copy.cells);
}

void gol_board_copy(Gol_Board *dst, size_t x, size_t y, Gol_Board *src) {
    if (x >= dst->width || y >= dst->height) {
        return;
    }

    for (size_t py = 0; py < src->height; ++py) {
        for (size_t px = 0; px < src->width; ++px) {
            size_t bx = ((px + x - src->width / 2) + dst->width) % dst->width;
            size_t by = ((py + y - src->height / 2) + dst->height) % dst->height;

            if (src->cells[py * src->width + px] == STATE_DEAD) {
                continue;
            }

            size_t index = by * dst->width + bx;
            dst->cells[index] = !dst->cells[index];
        }
    }
}

void gol_board_clear(Gol_Board *board) {
    memset(board->cells, STATE_DEAD, board->width * board->height * sizeof(Gol_State));
}

void gol_board_free(Gol_Board *board) {
    free(board->cells);
}
