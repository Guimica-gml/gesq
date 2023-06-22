#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "./ui.h"
#include "./gol.h"
#include "./patterns.h"

#define COLOR_BACKGROUND  (Color) { 0xBE, 0x93, 0xD4, 0xFF }
#define COLOR_ALIVE       (Color) { 0xA0, 0x20, 0xF0, 0xFF }
#define COLOR_DEAD        (Color) { 0xC8, 0xA2, 0xC8, 0xFF }
#define COLOR_LINE        (Color) { 0x78, 0x51, 0xA9, 0xFF }
#define COLOR_PREVIEW     (Color) { 0xA0, 0x20, 0xF0, 0x55 }

#define COLOR_BUTTON_NORMAL  (Color) { 0xC8, 0xA2, 0xC8, 0xFF }
#define COLOR_BUTTON_HOVER   (Color) { 0xC9, 0xA0, 0xDC, 0xFF }
#define COLOR_BUTTON_PRESSED (Color) { 0xCB, 0xC3, 0xE3, 0xFF }

#define COLOR_TEXT (Color) { 0x36, 0x01, 0x3F, 0xFF }

// milliseconds
#define UPDATE_INTERVAL 15

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 580

#define BOARD_DIM 29

#define ARRAY_LEN(arr) sizeof(arr) / sizeof((arr)[0])

// State of the program
Gol_Board board = { 0 };
Gol_Board pattern = { 0 };

bool paused = true;
double update_timer = UPDATE_INTERVAL;

void advance_button_pressed(void) {
    if (paused) {
        gol_board_advance(&board);
    }
}

void clear_button_pressed(void) {
    gol_board_clear(&board);
}

float minf(float x1, float x2) {
    return (x1 > x2) ? x2 : x1;
}

bool point_in_rect(Vector2 point, UI_Rect rect) {
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

void widget_label(UI_Rect rect, Font font, const char *text, float font_size) {
    float spacing = 1.0;
    Vector2 size = MeasureTextEx(font, text, font_size, spacing);

    Vector2 pos = { 0 };
    pos.x = rect.x + (rect.w - size.x) / 2;
    pos.y = rect.y + (rect.h - size.y) / 2;

    DrawTextEx(font, text, pos, font_size, spacing, COLOR_TEXT);
}

bool widget_button(UI_Rect rect, Font font, const char *text) {
    Vector2 mouse = GetMousePosition();
    Color color = COLOR_BUTTON_NORMAL;
    bool pressed = false;

    if (point_in_rect(mouse, rect)) {
        color = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? COLOR_BUTTON_PRESSED : COLOR_BUTTON_HOVER;
        pressed = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    }

    DrawRectangle(rect.x, rect.y, rect.w, rect.h, color);
    DrawRectangleLines(rect.x, rect.y, rect.w, rect.h, COLOR_LINE);
    widget_label(rect, font, text, 20.0);

    return pressed;
}

void widget_pattern_button(UI_Rect rect, Font font, const char *text, Gol_Board new_pattern) {
    bool pressed = widget_button(rect, font, text);
    if (pressed) {
        pattern = new_pattern;
    }
}

void widget_func_button(UI_Rect rect, Font font, const char *text, void (*func)(void)) {
    bool pressed = widget_button(rect, font, text);
    if (func != NULL && pressed) {
        func();
    }
}

void widget_toggle_button(UI_Rect rect, Font font, const char *text, bool *toggle) {
    Vector2 mouse = GetMousePosition();
    Color color = (*toggle) ? COLOR_BUTTON_PRESSED : COLOR_BUTTON_NORMAL;

    if (point_in_rect(mouse, rect)) {
        if (!(*toggle)) {
            color = COLOR_BUTTON_HOVER;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *toggle = !(*toggle);
        }
    }

    DrawRectangle(rect.x, rect.y, rect.w, rect.h, color);
    DrawRectangleLines(rect.x, rect.y, rect.w, rect.h, COLOR_LINE);
    widget_label(rect, font, text, 20.0);
}

int main(void) {
    board = gol_board_new(BOARD_DIM, BOARD_DIM);
    pattern = pattern_default;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gesq - nesq's game of life");
    SetTargetFPS(60);

    UI_Stack ui_stack = ui_stack_new();

    Font font = LoadFontEx("./font/iosevka.ttf", 40, NULL, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    while (!WindowShouldClose()) {
        float w = GetRenderWidth();
        float h = GetRenderHeight();

        float board_size = minf(w, h);
        float cell_size = board_size / BOARD_DIM;

        Vector2 mouse_pos = GetMousePosition();

        size_t cell_x = mouse_pos.x / cell_size;
        size_t cell_y = mouse_pos.y / cell_size;

        if (paused && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            gol_board_copy(&board, cell_x, cell_y, &pattern);
        }

        if (!paused) {
            update_timer -= 1;
            if (update_timer <= 0) {
                gol_board_advance(&board);
                update_timer = UPDATE_INTERVAL;
            }
        }

        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);

        DrawRectangle(0, 0, board_size, board_size, COLOR_DEAD);

        if (paused && cell_x < board.width && cell_y < board.height) {
            BeginBlendMode(BLEND_ALPHA);
            for (size_t y = 0; y < pattern.height; ++y) {
                for (size_t x = 0; x < pattern.width; ++x) {
                    size_t bx = ((x + cell_x - pattern.width / 2) + board.width) % board.width;
                    size_t by = ((y + cell_y - pattern.height / 2) + board.height) % board.height;

                    if (pattern.cells[y * pattern.width + x] == STATE_DEAD) {
                        continue;
                    }

                    Rectangle rect = {
                        .x = bx * cell_size,
                        .y = by * cell_size,
                        .width = cell_size,
                        .height = cell_size,
                    };
                    DrawRectangleRec(rect, COLOR_PREVIEW);
                }
            }
            EndBlendMode();
        }

        for (size_t y = 0; y < board.height; ++y) {
            for (size_t x = 0; x < board.width; ++x) {
                if (board.cells[y * board.width + x] == STATE_DEAD) {
                    continue;
                }

                Rectangle rect = {
                    .x = x * cell_size,
                    .y = y * cell_size,
                    .width = cell_size,
                    .height = cell_size,
                };
                DrawRectangleRec(rect, COLOR_ALIVE);
            }
        }

        for (int i = 1; i < BOARD_DIM + 1; ++i) {
            int pos = i * cell_size;
            DrawLine(pos, 0, pos, board_size, COLOR_LINE);
            DrawLine(0, pos, board_size, pos, COLOR_LINE);
        }

        UI_Rect rect = { board_size, 0, w - board_size, h + 1 };
        int margin = 5;
        float font_size = 35.0;

        ui_layout_begin(&ui_stack, rect, UI_VERT, 12, margin);

        widget_label(ui_layout_rect(&ui_stack), font, "Patterns", font_size);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "None", pattern_default);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Glider", pattern_glider);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Hammerhead", pattern_hammerhead);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Cloverleaf", pattern_cloverleaf);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Light Spaceship", pattern_light_spaceship);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Middle Spaceship", pattern_middle_spaceship);
        widget_pattern_button(ui_layout_rect(&ui_stack), font, "Heavy Spaceship", pattern_heavy_spaceship);

        widget_label(ui_layout_rect(&ui_stack), font, "State", font_size);
        widget_toggle_button(ui_layout_rect(&ui_stack), font, "Paused", &paused);
        widget_func_button(ui_layout_rect(&ui_stack), font, "Advance State", advance_button_pressed);
        widget_func_button(ui_layout_rect(&ui_stack), font, "Clear", clear_button_pressed);

        ui_layout_end(&ui_stack);

        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    gol_board_free(&board);

    return 0;
}
