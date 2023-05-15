#ifndef UI_H_
#define UI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define UI_NO_POS 0, 0, 0, 0
#define UI_CONTAINER_CAP 64
#define UI_BUTTON_GROUP_CAP 64

typedef struct UI_Theme UI_Theme;
typedef struct UI_Node_Container UI_Node_Container;
typedef struct UI_Node_Label UI_Node_Label;
typedef struct UI_Node_Button UI_Node_Button;
typedef struct UI_Node_Toggle_Button UI_Node_Toggle_Button;
typedef union UI_Node_As UI_Node_As;
typedef struct UI_Node UI_Node;
typedef struct UI_Alloc UI_Alloc;
typedef struct UI_Button_Group UI_Button_Group;
typedef size_t UI_Node_Index;

typedef enum UI_Orientation {
    UI_HORIZONTAL,
    UI_VERTICAL,
} UI_Orientation;

typedef enum {
    UI_BUTTON_NORMAL,
    UI_BUTTON_SELECTED,
    UI_BUTTON_PRESSED,
} UI_Button_State;

typedef enum {
    UI_NODE_CONTAINER,
    UI_NODE_LABEL,
    UI_NODE_BUTTON,
    UI_NODE_TOGGLE_BUTTON,
} UI_Node_Kind;

struct UI_Theme {
    TTF_Font *font;
    SDL_Color color_line;
    SDL_Color color_background;
    SDL_Color color_foreground;
    SDL_Color color_selected;
    SDL_Color color_pressed;
};

struct UI_Alloc {
    UI_Node *nodes;
    size_t size;
    size_t capacity;
};

struct UI_Node_Container {
    UI_Orientation orientation;
    UI_Node_Index children[UI_CONTAINER_CAP];
    size_t size;
};

struct UI_Node_Label {
    const char *text;
};

struct UI_Node_Button {
    const char *text;
    void (*when_pressed)();
    UI_Button_State state;
};

struct UI_Node_Toggle_Button {
    const char *text;
    void (*when_pressed)();
    UI_Button_State state;
    UI_Button_Group *button_group;
    Uint32 last_mouse_state;
};

union UI_Node_As {
    UI_Node_Container container;
    UI_Node_Label label;
    UI_Node_Button button;
    UI_Node_Toggle_Button toggle_button;
};

struct UI_Node {
    UI_Node_As as;
    UI_Node_Kind kind;

    int x;
    int y;
    int w;
    int h;
};

struct UI_Button_Group {
    UI_Node_Index buttons[UI_BUTTON_GROUP_CAP];
    size_t size;
};

void ui_update_node(UI_Alloc *alloc, UI_Node_Index node_index);
void ui_render_node(SDL_Renderer *renderer, UI_Theme *theme, UI_Alloc *alloc, UI_Node_Index node_index);

UI_Node_Index ui_alloc_node(UI_Alloc *alloc, UI_Node node);
UI_Node *ui_alloc_get_node(UI_Alloc *alloc, UI_Node_Index index);
void ui_alloc_free(UI_Alloc *alloc);

UI_Node_Index ui_new_label(UI_Alloc *alloc, int x, int y, int w, int h, const char *text);
UI_Node_Index ui_new_button(UI_Alloc *alloc, int x, int y, int w, int h, const char *text, void (*when_pressed)());
UI_Node_Index ui_new_container(
    UI_Alloc *alloc,
    int x, int y, int w, int h,
    int item_size,
    int padding,
    UI_Orientation ori,
    UI_Node_Index children[],
    size_t size
);
UI_Node_Index ui_new_toggle_button(
    UI_Alloc *alloc,
    int x, int y, int w, int h,
    const char *text,
    bool pressed,
    void (*when_pressed)(),
    UI_Button_Group *button_group
);

#endif // UI_H_
