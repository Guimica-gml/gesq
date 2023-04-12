#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define COLOR_BACKGROUND  0xC8, 0xA2, 0xC8, 0xFF
#define COLOR_LINE        0x78, 0x51, 0xA9, 0xFF
#define COLOR_ALIVE       0xA0, 0x20, 0xF0, 0xFF

// milliseconds (i guess)
#define UPDATE_INTERVAL 250

#define BOARD_DIM 13
#define CELL_SIZE 48
#define WINDOW_DIM (BOARD_DIM * CELL_SIZE)

typedef enum {
    STATE_DEAD = 0,
    STATE_ALIVE = 1,
} State;

typedef struct {
    State cells[BOARD_DIM][BOARD_DIM];
} Board;

int alive_neighbor_count(Board *board, size_t x, size_t y) {
    int neighbor_count = 0;

    for (int dy = -1; dy < 2; ++dy) {
        for (int dx = -1; dx < 2; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = x + dx;
            int ny = y + dy;

            if (nx > BOARD_DIM - 1) {
                nx = 0;
            }
            if (nx < 0) {
                nx = BOARD_DIM - 1;
            }
            if (ny > BOARD_DIM - 1) {
                ny = 0;
            }
            if (ny < 0) {
                ny = BOARD_DIM - 1;
            }

            neighbor_count += board->cells[ny][nx];
        }
    }

    return neighbor_count;
}

void advance_board(Board *board) {
    Board *board_copy = malloc(sizeof(Board));
    memcpy(board_copy, board, sizeof(Board));

    for (size_t y = 0; y < BOARD_DIM; ++y) {
        for (size_t x = 0; x < BOARD_DIM; ++x) {
            int neighbor_count = alive_neighbor_count(board_copy, x, y);
            if (board->cells[y][x] == STATE_DEAD) {
                if (neighbor_count == 3) {
                    board->cells[y][x] = STATE_ALIVE;
                }
            } else if (board->cells[y][x] == STATE_ALIVE) {
                if (neighbor_count != 2 && neighbor_count != 3) {
                    board->cells[y][x] = STATE_DEAD;
                }
            } else {
                assert(0 && "unreachable");
            }
        }
    }

    free(board_copy);
}

int main(void) {
    Board board = { 0 };

    // Bar
    board.cells[5][7] = STATE_ALIVE;
    board.cells[5][8] = STATE_ALIVE;
    board.cells[5][9] = STATE_ALIVE;

    // Glider
    board.cells[3][2] = STATE_ALIVE;
    board.cells[4][3] = STATE_ALIVE;
    board.cells[5][1] = STATE_ALIVE;
    board.cells[5][2] = STATE_ALIVE;
    board.cells[5][3] = STATE_ALIVE;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL Error: could not initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(
        "gesq - nesq's game of life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_DIM,
        WINDOW_DIM,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        fprintf(stderr, "SDL Error: could not create a window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        fprintf(stderr, "SDL Error: could not create a renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // State of the program
    bool paused = true;
    double update_timer = UPDATE_INTERVAL;

    // Delta time
    Uint64 now = SDL_GetPerformanceCounter();
    double delta_time = 0;

    // Game loop
    bool quit = false;
    SDL_Event event = { 0 };

    while (!quit) {
        Uint64 last = now;
        now = SDL_GetPerformanceCounter();
        delta_time = (double) ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        paused = !paused;
                    } else if (event.key.keysym.sym == SDLK_SPACE && paused) {
                        advance_board(&board);
                    }
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (paused && event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x / CELL_SIZE;
                        int y = event.button.y / CELL_SIZE;
                        board.cells[y][x] = !board.cells[y][x];
                    }
                } break;
            }
        }

        if (!paused) {
            update_timer -= delta_time;
            if (update_timer <= 0) {
                advance_board(&board);
                update_timer = UPDATE_INTERVAL;
            }
        }

        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, COLOR_ALIVE);
        for (size_t y = 0; y < BOARD_DIM; ++y) {
            for (size_t x = 0; x < BOARD_DIM; ++x) {
                if (board.cells[y][x] == STATE_DEAD) {
                    continue;
                }

                SDL_Rect rect = {
                    .x = x * CELL_SIZE,
                    .y = y * CELL_SIZE,
                    .w = CELL_SIZE,
                    .h = CELL_SIZE,
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, COLOR_LINE);
        for (int i = 1; i < BOARD_DIM; ++i) {
            int pos = i * CELL_SIZE;
            SDL_RenderDrawLine(renderer, pos, 0, pos, WINDOW_DIM);
            SDL_RenderDrawLine(renderer, 0, pos, WINDOW_DIM, pos);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
