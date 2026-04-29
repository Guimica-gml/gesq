#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <raylib.h>

#define UI_IMPLEMENTATION
#include "./ui.h"

#define GOL_IMPLEMENTATION
#include "./gol.h"

// Patterns are placed in another file to avoid cluttering main.c
#include "./patterns.c"

#define COLOR_BACKGROUND  (Color) { 0xBE, 0x93, 0xD4, 0xFF }
#define COLOR_ALIVE       (Color) { 0xA0, 0x20, 0xF0, 0xFF }
#define COLOR_DEAD        (Color) { 0xC8, 0xA2, 0xC8, 0xFF }
#define COLOR_LINE        (Color) { 0x78, 0x51, 0xA9, 0xFF }
#define COLOR_PREVIEW     (Color) { 0xA0, 0x20, 0xF0, 0x55 }

#define COLOR_PANEL_BACKGROUND (Color) { 0xCE, 0x73, 0xC3, 0xFF }
#define COLOR_BUTTON_NORMAL    (Color) { 0xC8, 0xA2, 0xC8, 0xFF }
#define COLOR_BUTTON_HOVER     (Color) { 0xC9, 0xA0, 0xDC, 0xFF }
#define COLOR_BUTTON_PRESSED   (Color) { 0xD3, 0xCC, 0xE6, 0xFF }

#define COLOR_TEXT (Color) { 0x36, 0x01, 0x3F, 0xFF }

#define PANEL_SCROLLBAR_SIZE 16
#define DRAW_PANEL_THRESHOLD 200

// in frames
#define UPDATE_INTERVAL 20

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define BOARD_DIM 30

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#define ui2rl(rect) ((Rectangle) { (rect).x, (rect).y, (rect).w, (rect).h })

Gol_Board board = {0};
Gol_Board pattern = pattern_default;

bool paused = true;
double update_timer = UPDATE_INTERVAL;

bool clipping_rect_active = false;
UI_Rect clipping_rect = {0};

void advance_button_pressed(void) {
	if (paused) {
		gol_board_advance(&board);
	}
}

void clear_button_pressed(void) {
	gol_board_clear(&board);
}

void widget_label(UI_Rect rect, Font font, const char *text, float font_size) {
	float spacing = 1.0;
	Vector2 size = MeasureTextEx(font, text, font_size, spacing);

	Vector2 pos = {0};
	pos.x = rect.x + (rect.w - size.x) / 2;
	pos.y = rect.y + (rect.h - size.y) / 2;

	DrawTextEx(font, text, pos, font_size, spacing, COLOR_TEXT);
}

static inline void enable_clipping_mode(UI_Rect rect) {
	clipping_rect = rect;
	clipping_rect_active = true;
}

static inline void disable_clipping_mode(void) {
	clipping_rect_active = false;
}

static inline bool ensure_clipping(Vector2 point) {
	return clipping_rect_active && CheckCollisionPointRec(point, ui2rl(clipping_rect));
}

bool widget_button(UI_Rect rect, Font font, const char *text) {
	Vector2 mouse = GetMousePosition();
	Color color = COLOR_BUTTON_NORMAL;
	bool pressed = false;

	if (ensure_clipping(mouse) && CheckCollisionPointRec(mouse, ui2rl(rect))) {
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			color = COLOR_BUTTON_PRESSED;
			pressed = true;
		} else {
			color = COLOR_BUTTON_HOVER;
		}
	}

	DrawRectangleRec(ui2rl(rect), color);
	DrawRectangleLinesEx(ui2rl(rect), 1, COLOR_LINE);
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

	if (ensure_clipping(mouse) && CheckCollisionPointRec(mouse, ui2rl(rect))) {
		if (!(*toggle)) {
			color = COLOR_BUTTON_HOVER;
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			*toggle = !(*toggle);
		}
	}

	DrawRectangleRec(ui2rl(rect), color);
	DrawRectangleLinesEx(ui2rl(rect), 1, COLOR_LINE);
	widget_label(rect, font, text, 20.0);
}

typedef struct {
	int scroll;
	bool is_dragging;
	int mouse_delta;
} Scroll_Panel_State;

UI_Layout widget_scrollable_panel_start(UI_Rect rect, int elem_height, int elem_count, Scroll_Panel_State *state) {
	int m = 5;
	int height = (elem_height + m) * elem_count + m;
	int max_scroll = height - rect.h;

	UI_Rect full_rect = rect;
	UI_Rect knob_rect = {0};
	UI_Rect scrollbar_rect = {0};

	ui_apply_margin(&rect, ui_marginv(m));
	scrollbar_rect.x = rect.x + rect.w - PANEL_SCROLLBAR_SIZE + m;
	scrollbar_rect.y = rect.y;
	scrollbar_rect.w = PANEL_SCROLLBAR_SIZE - m;
	scrollbar_rect.h = rect.h;
	rect.w -= PANEL_SCROLLBAR_SIZE;

	Vector2 mouse_pos = GetMousePosition();
	if (height > rect.h) {
		if (CheckCollisionPointRec(mouse_pos, ui2rl(full_rect))) {
			float wheel = GetMouseWheelMove();
			state->scroll += (int) (-wheel * 10);
		}

		if (state->is_dragging) {
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				state->is_dragging = false;
			}
		} else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse_pos, ui2rl(scrollbar_rect))) {
			state->is_dragging = true;
		}

		state->scroll = max(state->scroll, 0);
		state->scroll = min(state->scroll, max_scroll);

		float knob_scroll = (float) state->scroll / max_scroll;
		int knob_height = scrollbar_rect.h * ((float) rect.h / height);

		knob_rect.x = scrollbar_rect.x;
		knob_rect.y = scrollbar_rect.y + ((scrollbar_rect.h - knob_height) * knob_scroll);
		knob_rect.w = scrollbar_rect.w;
		knob_rect.h = knob_height;
	}

	DrawRectangleRec(ui2rl(full_rect), COLOR_PANEL_BACKGROUND);
	DrawRectangleLinesEx(ui2rl(full_rect), 1, COLOR_LINE);

	DrawRectangleRec(ui2rl(scrollbar_rect), PURPLE);
	DrawRectangleLinesEx(ui2rl(scrollbar_rect), 1, COLOR_LINE);

	if (height - (m * 2) > rect.h) {
		DrawRectangleRec(ui2rl(knob_rect), COLOR_DEAD);
		DrawRectangleLinesEx(ui2rl(knob_rect), 1, COLOR_LINE);
	}

	BeginScissorMode(rect.x - 1, rect.y - 1, rect.w + 1, rect.h + 1);
	enable_clipping_mode(rect);
	return ui_create_inc_layout(rect.x, rect.y - ((height > rect.h) ? state->scroll : 0), rect.w, elem_height, UI_VERT, m);
}

void widget_scrollable_panel_end() {
	disable_clipping_mode();
	EndScissorMode();
}

void widget_control_panels(UI_Rect rect, Font font) {
	UI_Rect patterns_rect, state_rect;
	ui_apply_margin(&rect, ui_marginv(5));
	ui_split_rectangle(rect, &patterns_rect, &state_rect, 0.65f, UI_VERT, 5);

	int elem_height = 40;
	float font_size = 35.0;

	static Scroll_Panel_State patterns_scroll_panel = {0};
	static Scroll_Panel_State state_scroll_panel = {0};

	UI_Layout layout = widget_scrollable_panel_start(patterns_rect, elem_height, 8, &patterns_scroll_panel);
	widget_label(ui_layout_next(&layout), font, "Patterns", font_size);
	widget_pattern_button(ui_layout_next(&layout), font, "None", pattern_default);
	widget_pattern_button(ui_layout_next(&layout), font, "Glider", pattern_glider);
	widget_pattern_button(ui_layout_next(&layout), font, "Hammerhead", pattern_hammerhead);
	widget_pattern_button(ui_layout_next(&layout), font, "Cloverleaf", pattern_cloverleaf);
	widget_pattern_button(ui_layout_next(&layout), font, "Light Spaceship", pattern_light_spaceship);
	widget_pattern_button(ui_layout_next(&layout), font, "Middle Spaceship", pattern_middle_spaceship);
	widget_pattern_button(ui_layout_next(&layout), font, "Heavy Spaceship", pattern_heavy_spaceship);
	widget_scrollable_panel_end();

	layout = widget_scrollable_panel_start(state_rect, elem_height, 4, &state_scroll_panel);
	widget_label(ui_layout_next(&layout), font, "State", font_size);
	widget_toggle_button(ui_layout_next(&layout), font, "Paused", &paused);
	widget_func_button(ui_layout_next(&layout), font, "Advance State", advance_button_pressed);
	widget_func_button(ui_layout_next(&layout), font, "Clear", clear_button_pressed);
	widget_scrollable_panel_end();
}

int main(void) {
	board = gol_board_new(BOARD_DIM, BOARD_DIM);

	SetTraceLogLevel(LOG_WARNING);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gesq - nesq's game of life");
	SetTargetFPS(60);

	Font font = LoadFontEx("./font/iosevka.ttf", 40, NULL, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	while (!WindowShouldClose()) {
		int w = GetRenderWidth();
		int h = GetRenderHeight();

		int board_side = min(w, h);
		int cell_size = board_side / BOARD_DIM;

		Vector2 mouse = GetMousePosition();

		int board_w = board.width * cell_size;
		int board_h = board.height * cell_size;

		bool draw_panel = false;
		UI_Rect panel_rect = {0};

		if (w > h) {
			int max_width =  400;
			int width = min(w - board_w, max_width);
			panel_rect = (UI_Rect) { w - width, 0, width, h + 1 };
			draw_panel = panel_rect.w > DRAW_PANEL_THRESHOLD;
		} else if (h > w) {
			panel_rect = (UI_Rect) { 0, board_h, w + 1, h - board_h };
			draw_panel = panel_rect.h > DRAW_PANEL_THRESHOLD;
		}

		int board_x = 0, board_y = 0;
		if (!draw_panel) {
			board_x = (w - board_w) / 2;
			board_y = (h - board_h) / 2;
		} else if (w > h) {
			board_x = ((w - panel_rect.w) - board_w) / 2;
			board_y = (board_side - board_h) / 2;
		} else if (h > w) {
			board_x = (board_side - board_w) / 2;
		}

		unsigned int cell_x = (mouse.x - board_x) / cell_size;
		unsigned int cell_y = (mouse.y - board_y) / cell_size;

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

		DrawRectangle(board_x, board_y, board_w, board_h, COLOR_DEAD);

		if (paused && cell_x < board.width && cell_y < board.height) {
			BeginBlendMode(BLEND_ALPHA);
			for (size_t y = 0; y < pattern.height; ++y) {
				for (size_t x = 0; x < pattern.width; ++x) {
					int bx = ((x + cell_x - pattern.width / 2) + board.width) % board.width;
					int by = ((y + cell_y - pattern.height / 2) + board.height) % board.height;

					if (pattern.cells[y * pattern.width + x] == STATE_ALIVE) {
						Rectangle rect = {0};
						rect.x = board_x + bx * cell_size;
						rect.y = board_y + by * cell_size;
						rect.width = cell_size;
						rect.height = cell_size;
						DrawRectangleRec(rect, COLOR_PREVIEW);
					}
				}
			}
			EndBlendMode();
		}

		for (size_t y = 0; y < board.height; ++y) {
			for (size_t x = 0; x < board.width; ++x) {
				if (board.cells[y * board.width + x] == STATE_ALIVE) {
					Rectangle rect = {0};
					rect.x = board_x + x * cell_size;
					rect.y = board_y + y * cell_size;
					rect.width = cell_size;
					rect.height = cell_size;
					DrawRectangleRec(rect, COLOR_ALIVE);
				}
			}
		}

		for (size_t i = 0; i < BOARD_DIM + 1; ++i) {
			int pos = i * cell_size;
			DrawLine(board_x + pos, board_y, board_x + pos, board_y + board_h, COLOR_LINE);
			DrawLine(board_x, board_y + pos, board_x + board_w, board_y + pos, COLOR_LINE);
		}

		if (draw_panel) {
			widget_control_panels(panel_rect, font);
		}

		EndDrawing();
	}

	UnloadFont(font);
	CloseWindow();
	gol_board_free(&board);

	return 0;
}
