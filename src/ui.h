#ifndef UI_H_
#define UI_H_

#include <stdlib.h>

typedef enum {
	UI_HORI,
	UI_VERT,
} UI_Orientation;

typedef struct {
	int x;
	int y;
	int w;
	int h;
} UI_Rect;

typedef struct {
	int top;
	int bottom;
	int left;
	int right;
} UI_Margin;

typedef enum {
	UI_FIXED,
	UI_INCREMENTAL,
} UI_Layout_Kind;

typedef struct {
	UI_Layout_Kind kind;
	UI_Orientation ori;
	size_t count;
	int gap;

	// Fixed
	UI_Rect rect;
	size_t capacity;

	// Incremental
	int x;
	int y;
	int elem_w;
	int elem_h;
} UI_Layout;

UI_Layout ui_create_fixed_layout(UI_Rect rect, size_t cap, UI_Orientation ori, int gap);
UI_Layout ui_create_inc_layout(int x, int y, int elem_w, int elem_h, UI_Orientation ori, int gap);

UI_Rect ui_layout_next(UI_Layout *layout);

UI_Margin ui_margin(int top, int bottom, int left, int right);
UI_Margin ui_marginv(int v);

void ui_apply_margin(UI_Rect *rect, UI_Margin margin);
void ui_split_rectangle(UI_Rect rect, UI_Rect *first, UI_Rect *second, float split, UI_Orientation orient, int gap);

#endif // UI_H_

#ifdef UI_IMPLEMENTATION
#undef UI_IMPLEMENTATION

#include <assert.h>

UI_Layout ui_create_fixed_layout(UI_Rect rect, size_t cap, UI_Orientation ori, int gap) {
	UI_Layout layout = {0};
	layout.kind = UI_FIXED;
	layout.rect = rect;
	layout.ori = ori;
	layout.capacity = cap;
	layout.gap = gap;
	return layout;
}

UI_Layout ui_create_inc_layout(int x, int y, int elem_w, int elem_h, UI_Orientation ori, int gap) {
	UI_Layout layout = {0};
	layout.kind = UI_INCREMENTAL;
	layout.x = x;
	layout.y = y;
	layout.elem_w = elem_w;
	layout.elem_h = elem_h;
	layout.ori = ori;
	layout.gap = gap;
	return layout;
}

UI_Rect ui_layout_next(UI_Layout *lo) {
	UI_Rect rect = {0};
	switch (lo->kind) {
	case UI_FIXED: {
		switch (lo->ori) {
		case UI_HORI: {
			int slot_size = (lo->rect.w - (lo->gap * (lo->capacity - 1))) / lo->capacity;
			rect.x = lo->rect.x  + ((slot_size + lo->gap) * lo->count);
			rect.y = lo->rect.y;
			rect.w = slot_size;
			rect.h = lo->rect.h;
		} break;
		case UI_VERT: {
			int slot_size = (lo->rect.h - (lo->gap * (lo->capacity - 1))) / lo->capacity;
			rect.x = lo->rect.x;
			rect.y = lo->rect.y + ((slot_size + lo->gap) * lo->count);
			rect.w = lo->rect.w;
			rect.h = slot_size;
		} break;
		}
	} break;
	case UI_INCREMENTAL: {
		switch (lo->ori) {
		case UI_HORI: {
			rect.x = lo->x + ((lo->elem_w + lo->gap) * lo->count);
			rect.y = lo->y;
			rect.w = lo->elem_w;
			rect.h = lo->elem_h;
		} break;
		case UI_VERT: {
			rect.x = lo->x;
			rect.y = lo->y + ((lo->elem_h + lo->gap) * lo->count);
			rect.w = lo->elem_w;
			rect.h = lo->elem_h;
		} break;
		}
	} break;
	}
	lo->count += 1;
	return rect;
}

UI_Margin ui_margin(int top, int bottom, int left, int right) {
	return (UI_Margin) { top, bottom, left, right };
}

UI_Margin ui_marginv(int v) {
	return (UI_Margin) { v, v, v, v };
}

void ui_apply_margin(UI_Rect *rect, UI_Margin margin) {
	rect->x += margin.left;
	rect->y += margin.top;
	rect->w -= (margin.left + margin.right);
	rect->h -= (margin.top + margin.bottom);
}

void ui_split_rectangle(UI_Rect rect, UI_Rect *first, UI_Rect *second, float split, UI_Orientation orient, int gap) {
	assert(first != NULL);
	assert(second != NULL);
	assert(0.0f <= split && split <= 1.0f);

	int gap_fst = gap / 2;
	int gap_scd = gap_fst;
	if ((gap % 2) != 0) {
		gap_scd += 1;
	}

	switch (orient) {
	case UI_HORI: {
		first->x = rect.x;
		first->y = rect.y;
		first->w = rect.w * split - gap_fst;
		first->h = rect.h;

		second->x = rect.x + first->w + gap_fst + gap_scd;
		second->y = rect.y;
		second->w = rect.w - first->w - gap_fst - gap_scd;
		second->h = rect.h;
	} break;
	case UI_VERT: {
		first->x = rect.x;
		first->y = rect.y;
		first->w = rect.w;
		first->h = rect.h * split - gap_fst;

		second->x = rect.x;
		second->y = rect.y + first->h + gap_fst + gap_scd;
		second->w = rect.w;
		second->h = rect.h - first->h - gap_fst - gap_scd;
	} break;
	}
}

#endif // UI_IMPLEMENTATION
