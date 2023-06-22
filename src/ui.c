#include <assert.h>
#include "./ui.h"

UI_Stack ui_stack_new(void) {
    return (UI_Stack) { 0 };
}

void ui_stack_free(UI_Stack *stack) {
    free(stack->items);
}

void ui_layout_begin(UI_Stack *stack, UI_Rect rect, UI_Orientation ori, size_t cap, int margin) {
    UI_Layout layout = { 0 };
    layout.rect = rect;
    layout.ori = ori;
    layout.capacity = cap;
    layout.margin = margin;
    da_append(stack, layout);
}

void ui_layout_end(UI_Stack *stack) {
    assert(stack->count > 0);
    stack->count -= 1;
}

UI_Rect ui_layout_rect(UI_Stack *stack) {
    UI_Rect rect = { 0 };
    UI_Layout *layout = &stack->items[stack->count - 1];

    switch (layout->ori) {
        case UI_HORI: {
            assert(0 && "unimplemented");
        } break;
        case UI_VERT: {
            int slot_size = ((layout->rect.h - layout->margin) / layout->capacity) - layout->margin;
            rect.x = layout->rect.x + layout->margin;
            rect.y = layout->rect.y + (slot_size * layout->count) + (layout->margin * (layout->count + 1));
            rect.w = layout->rect.w - (layout->margin * 2);
            rect.h = slot_size;
        } break;
    }

    layout->count += 1;
    return rect;
}
