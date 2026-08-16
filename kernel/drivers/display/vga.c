#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM ((volatile uint16_t*)0xB8000)
#define NUM_TTYS 4

typedef struct {
    uint8_t cursor_x;
    uint8_t cursor_y;
    uint8_t color;
    uint16_t buffer[VGA_WIDTH * VGA_HEIGHT];
} tty_t;

static tty_t ttys[NUM_TTYS];
static uint8_t active_tty = 0; 

void tty_init(void) {
    for (int i = 0; i < NUM_TTYS; i++) {
        ttys[i].cursor_x = 0;
        ttys[i].cursor_y = 0;
        ttys[i].color = 0x0F; // White on black
        
        for (int j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++) {
            ttys[i].buffer[j] = (0x0F << 8) | ' ';
        }
    }
}

static void scroll(tty_t *tty) {
    if (tty->cursor_y >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            tty->buffer[i] = tty->buffer[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            tty->buffer[i] = (tty->color << 8) | ' ';
        }
        tty->cursor_y = VGA_HEIGHT - 1;
    }
}

void tty_flush(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEM[i] = ttys[active_tty].buffer[i];
    }
}

void tty_switch(uint8_t tty_index) {
    if (tty_index >= NUM_TTYS || tty_index == active_tty) return;
    active_tty = tty_index;
    tty_flush();
}

void tty_putchar_id(uint8_t id, char c) {
    if (id >= NUM_TTYS) return;
    tty_t *tty = &ttys[id];

    if (c == '\n') {
        tty->cursor_x = 0;
        tty->cursor_y++;
    } else if (c == '\b') {
        if (tty->cursor_x > 0) {
            tty->cursor_x--;
            tty->buffer[tty->cursor_y * VGA_WIDTH + tty->cursor_x] = (tty->color << 8) | ' ';
        }
    } else {
        tty->buffer[tty->cursor_y * VGA_WIDTH + tty->cursor_x] = (tty->color << 8) | (unsigned char)c;
        tty->cursor_x++;
        if (tty->cursor_x >= VGA_WIDTH) {
            tty->cursor_x = 0;
            tty->cursor_y++;
        }
    }
    scroll(tty);

    if (id == active_tty) {
        tty_flush();
    }
}


void tty_putchar(char c) {
    tty_putchar_id(active_tty, c);
}

void tty_puts(const char *str) {
    while (*str) {
        tty_putchar(*str++);
    }
}

uint8_t tty_get_active(void) {
    return active_tty;
}