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
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 0x86, '^', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0x94, 0x84, '\'',
    0,  '<', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
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
                const char* table = (current_layout == LAYOUT_FI) ? scancode_fi : scancode_us;
                if (scancode < sizeof(scancode_us)) {
                    char c = table[scancode];
                    if (c != 0) return c;
                }
            }
        }
    }
}


static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;

void tty_switch(uint8_t tty_index); 

void keyboard_handle_scancode(uint8_t scancode) {

    uint8_t is_released = scancode & 0x80;
    uint8_t code = scancode & 0x7F;


    if (code == 0x1D) {
        ctrl_pressed = !is_released;
        return;
    }
    if (code == 0x38) {
        alt_pressed = !is_released;
        return;
    }


    if (!is_released) {
        if (ctrl_pressed && alt_pressed) {
            switch (code) {
                case 0x3B: tty_switch(0); return; // Ctrl+Alt+F1 -> tty1
                case 0x3C: tty_switch(1); return; // Ctrl+Alt+F2 -> tty2
                case 0x3D: tty_switch(2); return; // Ctrl+Alt+F3 -> tty3
                case 0x3E: tty_switch(3); return; // Ctrl+Alt+F4 -> tty4
            }
        }
        
        // Pass normal keystroke to active TTY input buffer here...
    }
}