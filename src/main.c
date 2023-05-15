#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "./ui.h"
#include "./gol.h"
#include "./patterns.h"

#define COLOR_BACKGROUND  0xBE, 0x93, 0xD4, 0xFF
#define COLOR_DEAD        0xC8, 0xA2, 0xC8, 0xFF
#define COLOR_LINE        0x78, 0x51, 0xA9, 0xFF
#define COLOR_ALIVE       0xA0, 0x20, 0xF0, 0xFF
#define COLOR_HOVER       0xA0, 0x20, 0xF0, 0x55
#define COLOR_PATTERN_PREVIEW 0xA0, 0x20, 0xF0, 0xAA

#define UI_COLOR_LINE        COLOR_LINE
#define UI_COLOR_BACKGROUND  COLOR_DEAD
#define UI_COLOR_FOREGROUND  0x36, 0x01, 0x3F, 0xFF
#define UI_COLOR_SELECTED    0xC9, 0xA0, 0xDC, 0xFF
#define UI_COLOR_PRESSED     0xCB, 0xC3, 0xE3, 0xFF
#define UI_PADDING 10

// milliseconds
#define UPDATE_INTERVAL 250

#define WINDOW_WIDTH 950
#define WINDOW_HEIGHT 725

#if WINDOW_WIDTH < WINDOW_HEIGHT
    #define BOARD_SIZE WINDOW_WIDTH
    #define UI_X 0
    #define UI_Y BOARD_SIZE
#else
    #define BOARD_SIZE WINDOW_HEIGHT
    #define UI_X BOARD_SIZE
    #define UI_Y 0
#endif

#define UI_W (WINDOW_WIDTH - UI_X)
#define UI_H (WINDOW_HEIGHT - UI_Y)

#define BOARD_DIM 25
#define CELL_SIZE (BOARD_SIZE / BOARD_DIM)

#define ARRAY_LEN(arr) sizeof(arr) / sizeof((arr)[0])

// State of the program
Gol_Board board = { 0 };
Gol_Board pattern = { 0 };

bool paused = true;
double update_timer = UPDATE_INTERVAL;

void ui_pause_button_pressed(UI_Node_Toggle_Button *toggle_button, bool pressed) {
    if (pressed) {
        toggle_button->text = "Click to Resume";
        paused = true;
    } else {
        toggle_button->text = "Click to Pause";
        paused = false;
    }
}

void ui_advance_button_pressed(void) {
    if (paused) {
        gol_board_advance(&board);
    }
}

void ui_clear_button_pressed(void) {
    gol_board_clear(&board);
}

void ui_pattern_none_button_pressed(void) {
    gol_board_resize(&pattern, PAT_DEFAULT_W, PAT_DEFAULT_W, pattern_default);
}

void ui_pattern_glider_button_pressed(void) {
    gol_board_resize(&pattern, PAT_GLIDER_W, PAT_GLIDER_H, pattern_glider);
}

void ui_pattern_hammerhead_button_pressed(void) {
    gol_board_resize(&pattern, PAT_HAMMERHEAD_W, PAT_HAMMERHEAD_H, pattern_hammerhead);
}

void ui_pattern_cloverleaf_button_pressed(void) {
    gol_board_resize(&pattern, PAT_CLOVERLEAF_W, PAT_CLOVERLEAF_H, pattern_cloverleaf);
}

void ui_pattern_light_spaceship_button_pressed(void) {
    gol_board_resize(&pattern, PAT_LIGHT_SPACESHIP_W, PAT_LIGHT_SPACESHIP_H, pattern_light_spaceship);
}

void ui_pattern_midlle_spaceship_button_pressed(void) {
    gol_board_resize(&pattern, PAT_MIDDLE_SPACESHIP_W, PAT_MIDDLE_SPACESHIP_H, pattern_middle_spaceship);
}

void ui_pattern_heavy_spaceship_button_pressed(void) {
    gol_board_resize(&pattern, PAT_HEAVY_SPACESHIP_W, PAT_HEAVY_SPACESHIP_H, pattern_heavy_spaceship);
}

int main(void) {
    board = gol_board_new(BOARD_DIM, BOARD_DIM);
    gol_board_resize(&pattern, PAT_DEFAULT_W, PAT_DEFAULT_W, pattern_default);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL Error: could not initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL Error: could not initialize SDL2_ttf: %s\n", TTF_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(
        "gesq - nesq's game of life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
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

    TTF_Font *font = TTF_OpenFont("./font/iosevka.ttf", 20);

    if (font == NULL) {
        fprintf(stderr, "SDL Error: could not open font: %s\n", TTF_GetError());
        exit(1);
    }

    // Otherwise alpha wont work :)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // UI
    UI_Alloc ui_alloc = { 0 };
    UI_Theme theme = {
        .font = font,
        .color_line = (SDL_Color) { UI_COLOR_LINE },
        .color_background = (SDL_Color) { UI_COLOR_BACKGROUND },
        .color_foreground = (SDL_Color) { UI_COLOR_FOREGROUND },
        .color_selected = (SDL_Color) { UI_COLOR_SELECTED },
        .color_pressed = (SDL_Color) { UI_COLOR_PRESSED },
    };

    UI_Node_Index ui_items[] = {
        ui_new_label(&ui_alloc, UI_NO_POS, "Patterns"),
        ui_new_button(&ui_alloc, UI_NO_POS, "None", ui_pattern_none_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Glider", ui_pattern_glider_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Hammerhead", ui_pattern_hammerhead_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Cloverleaf", ui_pattern_cloverleaf_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Light Spaceship", ui_pattern_light_spaceship_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Midlle Spaceship", ui_pattern_midlle_spaceship_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Heavy Spaceship", ui_pattern_heavy_spaceship_button_pressed),

        ui_new_label(&ui_alloc, UI_NO_POS, "State"),
        ui_new_toggle_button(&ui_alloc, UI_NO_POS, "Click to Resume", paused, ui_pause_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Advance State", ui_advance_button_pressed),
        ui_new_button(&ui_alloc, UI_NO_POS, "Clear", ui_clear_button_pressed),
    };

    UI_Node_Index ui_root = ui_new_container(&ui_alloc, UI_X, UI_Y, UI_W, UI_H, 40, 10, UI_VERTICAL, ui_items, ARRAY_LEN(ui_items));

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
                case SDL_MOUSEBUTTONDOWN: {
                    if (paused && event.button.button == SDL_BUTTON_LEFT) {
                        size_t cell_x = event.button.x / CELL_SIZE;
                        size_t cell_y = event.button.y / CELL_SIZE;
                        gol_board_copy(&board, cell_x, cell_y, &pattern);
                    }
                } break;
            }
        }

        if (!paused) {
            update_timer -= delta_time;
            if (update_timer <= 0) {
                gol_board_advance(&board);
                update_timer = UPDATE_INTERVAL;
            }
        }

        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, COLOR_DEAD);
        SDL_Rect bg_rect = {
            .x = 0,
            .y = 0,
            .w = BOARD_SIZE,
            .h = BOARD_SIZE,
        };
        SDL_RenderFillRect(renderer, &bg_rect);

        int mouse_x = 0;
        int mouse_y = 0;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        size_t cell_x = mouse_x / CELL_SIZE;
        size_t cell_y = mouse_y / CELL_SIZE;

        SDL_SetRenderDrawColor(renderer, COLOR_HOVER);
        if (paused && cell_x < board.width && cell_y < board.height) {
            for (size_t y = 0; y < pattern.height; ++y) {
                for (size_t x = 0; x < pattern.width; ++x) {
                    size_t bx = ((x + cell_x - pattern.width / 2) + board.width) % board.width;
                    size_t by = ((y + cell_y - pattern.height / 2) + board.height) % board.height;

                    if (pattern.cells[y * pattern.width + x] == STATE_DEAD) {
                        continue;
                    }

                    SDL_Rect rect = {
                        .x = bx * CELL_SIZE,
                        .y = by * CELL_SIZE,
                        .w = CELL_SIZE,
                        .h = CELL_SIZE,
                    };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, COLOR_ALIVE);
        for (size_t y = 0; y < board.height; ++y) {
            for (size_t x = 0; x < board.width; ++x) {
                if (board.cells[y * board.width + x] == STATE_DEAD) {
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
        for (int i = 1; i < BOARD_DIM + 1; ++i) {
            int pos = i * CELL_SIZE;
            SDL_RenderDrawLine(renderer, pos, 0, pos, BOARD_SIZE);
            SDL_RenderDrawLine(renderer, 0, pos, BOARD_SIZE, pos);
        }

        ui_update_node(&ui_alloc, ui_root);
        ui_render_node(renderer, &theme, &ui_alloc, ui_root);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    ui_alloc_free(&ui_alloc);
    gol_board_free(&board);
    gol_board_free(&pattern);

    return 0;
}
