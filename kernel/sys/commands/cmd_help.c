#include "drivers/vga.h"

void cmd_help(const char *args) {
    (void)args;
    tty_puts("Commands:\n");
    tty_puts("  help    - Show available options\n");
    tty_puts("  meminfo - Display physical memory usage\n");
    tty_puts("  wall    - Broadcast a message across all TTYs\n");
}