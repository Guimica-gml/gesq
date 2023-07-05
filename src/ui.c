#include <assert.h>
#include "./ui.h"

UI_Margin ui_margin(int top, int bottom, int left, int right) {
    return (UI_Margin) { top, bottom, left, right };
}

UI_Margin ui_marginv(int v) {
    return (UI_Margin) { v, v, v, v };
}

UI_Stack ui_stack_new(void) {
    return (UI_Stack) { 0 };
}

void ui_stack_free(UI_Stack *stack) {
    free(stack->items);
}

void ui_layout_begin(UI_Stack *stack, UI_Rect rect, UI_Orientation ori, UI_Margin margin, int gap, size_t cap) {
    UI_Layout layout = { 0 };
    layout.rect = rect;
    layout.ori = ori;
    layout.capacity = cap;
    layout.margin = margin;
    layout.gap = gap;
    da_append(stack, layout);
}

void ui_layout_end(UI_Stack *stack) {
    assert(stack->count > 0);
    stack->count -= 1;
}

UI_Rect ui_layout_rect(UI_Stack *stack) {
    UI_Rect rect = { 0 };
    UI_Layout *lo = &stack->items[stack->count - 1];

    switch (lo->ori) {
        case UI_HORI: {
            int slot_size = ((lo->rect.w - lo->margin.left - lo->margin.right) - (lo->gap * (lo->capacity - 1))) / lo->capacity;
            rect.x = lo->rect.x + lo->margin.left + ((slot_size + lo->gap) * lo->count);
            rect.y = lo->rect.y + lo->margin.top;
            rect.w = slot_size;
            rect.h = lo->rect.h - lo->margin.top - lo->margin.bottom;
        } break;
        case UI_VERT: {
            int slot_size = ((lo->rect.h - lo->margin.bottom - lo->margin.top) - (lo->gap * (lo->capacity - 1))) / lo->capacity;
            rect.x = lo->rect.x + lo->margin.left;
            rect.y = lo->rect.y + lo->margin.top + ((slot_size + lo->gap) * lo->count);
            rect.w = lo->rect.w - lo->margin.right - lo->margin.left;
            rect.h = slot_size;
        } break;
    }

    lo->count += 1;
    return rect;
}
