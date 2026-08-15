#include "drivers/keyboard.h"
#include "arch/x86_64/io.h"

static kb_layout_t current_layout = LAYOUT_US; // Default layout


static const char scancode_us[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',   0, ' '
};


static const char scancode_fi[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '+', '`', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'å', '^', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö', 'ä', '\'',
     0, '<', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
   '*',   0, ' '
};

void keyboard_set_layout(kb_layout_t layout) {
    current_layout = layout;
}

kb_layout_t keyboard_get_layout(void) {
    return current_layout;
}

char keyboard_getchar(void) {
    while (1) {

        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);          
  
            if (!(scancode & 0x80)) {
                const char* table = (current_layout == LAYOUT_FINNISH) ? scancode_fi : scancode_us;
                if (scancode < sizeof(scancode_us)) {
                    char c = table[scancode];
                    if (c != 0) return c;
                }
            }
        }
    }
}