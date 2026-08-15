#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

typedef enum {
    LAYOUT_US,
    LAYOUT_FINNISH
} kb_layout_t;

void keyboard_set_layout(kb_layout_t layout);
kb_layout_t keyboard_get_layout(void);
char keyboard_getchar(void);

#endif