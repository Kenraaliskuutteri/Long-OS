#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
    LAYOUT_US = 0,
    LAYOUT_FI = 1
} kb_layout_t;

void keyboard_set_layout(kb_layout_t layout);
kb_layout_t keyboard_get_layout(void);
char keyboard_getchar(void);

#endif