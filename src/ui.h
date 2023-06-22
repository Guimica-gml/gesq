#ifndef UI_H_
#define UI_H_

#include <stdlib.h>

#define DA_INIT_CAP 256
#define da_append(da, item)                                                          \
    do {                                                                             \
        if ((da)->count >= (da)->capacity) {                                         \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;   \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Buy more RAM lol");                       \
        }                                                                            \
                                                                                     \
        (da)->items[(da)->count++] = (item);                                         \
    } while (0)

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
    UI_Rect rect;
    UI_Orientation ori;
    size_t capacity;
    size_t count;
    int margin;
} UI_Layout;

typedef struct {
    UI_Layout *items;
    size_t capacity;
    size_t count;
} UI_Stack;

UI_Stack ui_stack_new(void);
void ui_stack_free(UI_Stack *stack);

void ui_layout_begin(UI_Stack *stack, UI_Rect rect, UI_Orientation ori, size_t cap, int margin);
void ui_layout_end(UI_Stack *stack);

UI_Rect ui_layout_rect(UI_Stack *stack);

#endif // UI_H_
