#include <stdio.h>
#include <assert.h>

#include "./ui.h"

#define UNPACK_COLOR(color) (color).r, (color).g, (color).b, (color).a

void sdl_draw_text(SDL_Renderer *renderer, UI_Theme *theme, int x, int y, const char *text) {
    SDL_Surface* surface = TTF_RenderText_Blended(theme->font, text, theme->color_foreground);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect rect = {
        .x = x - (w / 2),
        .y = y - (h / 2),
        .w = w,
        .h = h,
    };

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void ui_update_node(UI_Alloc *alloc, UI_Node_Index node_index) {
    int x, y;
    Uint32 button_state = SDL_GetMouseState(&x, &y);
    UI_Node *node = ui_alloc_get_node(alloc, node_index);

    switch (node->kind) {
        case UI_NODE_TOGGLE_BUTTON: {
            UI_Node_Toggle_Button *toggle_button = &node->as.toggle_button;
            if (x > node->x && y > node->y && x < node->x + node->w && y < node->y + node->h) {
                if ((button_state & SDL_BUTTON_LEFT) && ((toggle_button->last_mouse_state & SDL_BUTTON_LEFT) != 1)) {
                    if (toggle_button->state == UI_BUTTON_PRESSED) {
                        toggle_button->state = UI_BUTTON_SELECTED;
                    } else {
                        toggle_button->state = UI_BUTTON_PRESSED;
                    }
                    if (toggle_button->button_group != NULL) {
                        UI_Button_Group *button_group = toggle_button->button_group;
                        for (size_t i = 0; i < button_group->size; ++i) {
                            if (button_group->buttons[i] == node_index) {
                                continue;
                            }
                            UI_Node *node = ui_alloc_get_node(alloc, button_group->buttons[i]);
                            UI_Node_Toggle_Button *toggle_button = &node->as.toggle_button;
                            toggle_button->state = UI_BUTTON_NORMAL;
                        }
                    }
                    if (toggle_button->when_pressed != NULL) {
                        toggle_button->when_pressed(toggle_button, toggle_button->state == UI_BUTTON_PRESSED);
                    }
                } else {
                    if (toggle_button->state != UI_BUTTON_PRESSED) {
                        toggle_button->state = UI_BUTTON_SELECTED;
                    }
                }
            } else {
                if (toggle_button->state != UI_BUTTON_PRESSED) {
                    toggle_button->state = UI_BUTTON_NORMAL;
                }
            }
            toggle_button->last_mouse_state = button_state;
        } break;
        case UI_NODE_BUTTON: {
            UI_Node_Button *button = &node->as.button;
            if (x > node->x && y > node->y && x < node->x + node->w && y < node->y + node->h) {
                if (button_state & SDL_BUTTON_LEFT) {
                    button->state = UI_BUTTON_PRESSED;
                } else {
                    if (button->when_pressed != NULL && button->state == UI_BUTTON_PRESSED) {
                        button->when_pressed();
                    }
                    button->state = UI_BUTTON_SELECTED;
                }
            } else {
                button->state = UI_BUTTON_NORMAL;
            }
        } break;
        case UI_NODE_CONTAINER: {
            for (size_t i = 0; i < node->as.container.size; ++i) {
                ui_update_node(alloc, node->as.container.children[i]);
            }
        } break;
        case UI_NODE_LABEL: {
        } break;
    }
}

void ui_render_node(SDL_Renderer *renderer, UI_Theme *theme, UI_Alloc *alloc, UI_Node_Index node_index) {
    UI_Node *node = ui_alloc_get_node(alloc, node_index);

    SDL_Rect rect = {
        .x = node->x,
        .y = node->y,
        .w = node->w,
        .h = node->h,
    };

    switch (node->kind) {
        case UI_NODE_TOGGLE_BUTTON:
        case UI_NODE_BUTTON: {
            switch (node->as.button.state) {
                case UI_BUTTON_NORMAL: {
                    SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(theme->color_background));
                } break;
                case UI_BUTTON_SELECTED: {
                    SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(theme->color_selected));
                } break;
                case UI_BUTTON_PRESSED: {
                    SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(theme->color_pressed));
                } break;
            }

            SDL_RenderFillRect(renderer, &rect);

            int fnt_x = node->x + (node->w / 2);
            int fnt_y = node->y + (node->h / 2);
            sdl_draw_text(renderer, theme, fnt_x, fnt_y, node->as.button.text);

            SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(theme->color_line));
            SDL_RenderDrawRect(renderer, &rect);
        } break;
        case UI_NODE_CONTAINER: {
            for (size_t i = 0; i < node->as.container.size; ++i) {
                ui_render_node(renderer, theme, alloc, node->as.container.children[i]);
            }
        } break;
        case UI_NODE_LABEL: {
            int fnt_x = node->x + (node->w / 2);
            int fnt_y = node->y + (node->h / 2);
            sdl_draw_text(renderer, theme, fnt_x, fnt_y, node->as.label.text);
        } break;
    }
}

UI_Node_Index ui_alloc_node(UI_Alloc *alloc, UI_Node node) {
    if (alloc->size >= alloc->capacity) {
        alloc->capacity = (alloc->capacity == 0) ? sizeof(UI_Node) : alloc->capacity * 2;
        alloc->nodes = realloc(alloc->nodes, alloc->capacity * sizeof(UI_Node));
        if (alloc->nodes == NULL) {
            fprintf(stderr, "Error: not enough memory, buy more ram\n");
            exit(1);
        }
    }

    alloc->nodes[alloc->size++] = node;
    return alloc->size - 1;
}

UI_Node *ui_alloc_get_node(UI_Alloc *alloc, UI_Node_Index index) {
    assert(index < alloc->size);
    return &alloc->nodes[index];
}

void ui_alloc_free(UI_Alloc *alloc) {
    free(alloc->nodes);
}

UI_Node_Index ui_new_container(
    UI_Alloc *alloc,
    int x, int y, int w, int h,
    int item_size,
    int padding,
    UI_Orientation ori,
    UI_Node_Index children[],
    size_t size
) {
    assert(size < UI_CONTAINER_CAP);

    UI_Node node = { 0 };
    UI_Node_Container container = { 0 };

    node.x = x;
    node.y = y;
    node.w = w;
    node.h = h;
    container.orientation = ori;
    container.size = size;

    switch (ori) {
        case UI_HORIZONTAL: {
            for (size_t i = 0; i < size; ++i) {
                UI_Node *child = ui_alloc_get_node(alloc, children[i]);
                child->x = x + (padding * (i + 1)) + (item_size * i);
                child->y = y + padding;
                child->w = item_size;
                child->h = h - padding * 2;
                container.children[i] = children[i];
            }
        } break;
        case UI_VERTICAL: {
            for (size_t i = 0; i < size; ++i) {
                UI_Node *child = ui_alloc_get_node(alloc, children[i]);
                child->x = x + padding;
                child->y = y + (padding * (i + 1)) + (item_size * i);
                child->w = w - padding * 2;
                child->h = item_size;
                container.children[i] = children[i];
            }
        } break;
    }

    node.kind = UI_NODE_CONTAINER;
    node.as.container = container;
    return ui_alloc_node(alloc, node);
}

UI_Node_Index ui_new_label(UI_Alloc *alloc, int x, int y, int w, int h, const char *text) {
    UI_Node node = { 0 };
    UI_Node_Label label = { 0 };

    node.x = x;
    node.y = y;
    node.w = w;
    node.h = h;
    label.text = text;

    node.kind = UI_NODE_LABEL;
    node.as.label = label;
    return ui_alloc_node(alloc, node);
}

UI_Node_Index ui_new_button(UI_Alloc *alloc, int x, int y, int w, int h, const char *text, void (*when_pressed)()) {
    UI_Node node = { 0 };
    UI_Node_Button button = { 0 };

    node.x = x;
    node.y = y;
    node.w = w;
    node.h = h;
    button.text = text;
    button.when_pressed = when_pressed;

    node.kind = UI_NODE_BUTTON;
    node.as.button = button;
    return ui_alloc_node(alloc, node);
}

UI_Node_Index ui_new_toggle_button(
    UI_Alloc *alloc,
    int x, int y, int w, int h,
    const char *text,
    bool pressed,
    void (*when_pressed)(UI_Node_Toggle_Button *toggle_button, bool pressed),
    UI_Button_Group *button_group
) {
    UI_Node node = { 0 };
    UI_Node_Toggle_Button toggle_button = { 0 };

    node.x = x;
    node.y = y;
    node.w = w;
    node.h = h;
    toggle_button.text = text;
    toggle_button.when_pressed = when_pressed;
    toggle_button.button_group = button_group;

    if (pressed) {
        toggle_button.state = UI_BUTTON_PRESSED;
    } else {
        toggle_button.state = UI_BUTTON_NORMAL;
    }

    node.kind = UI_NODE_TOGGLE_BUTTON;
    node.as.toggle_button = toggle_button;
    UI_Node_Index node_index = ui_alloc_node(alloc, node);

    if (button_group != NULL) {
        assert(button_group->size < UI_BUTTON_GROUP_CAP);
        button_group->buttons[button_group->size++] = node_index;
    }

    return node_index;
}
