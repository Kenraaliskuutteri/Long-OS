#include "drivers/keyboard.h"
#include "arch/x86_64/io.h"

void handle_command(const char *input_buffer) {
    if (strcmp(input_buffer, "help") == 0) {
        vga_print("Commands: help, echo <text>, keyb [us|fi]", 2);
    } 
    else if (strcmp_prefix(input_buffer, "echo ")) {
        vga_print(input_buffer + 5, 2);
    } 
    else if (strcmp(input_buffer, "keyb") == 0) {
        kb_layout_t current = keyboard_get_layout();
        if (current == LAYOUT_FI) {
            vga_print("Current layout: FI (Finnish)", 2);
        } else {
            vga_print("Current layout: US (Default)", 2);
        }
        // This really fries my brains with how the commands are made, and all together the shell. But if it works, it works.
    } 
    else if (strcmp(input_buffer, "keyb fi") == 0) {
        keyboard_set_layout(LAYOUT_FI);
        vga_print("Switched layout to Finnish", 2);
    } 
    else if (strcmp(input_buffer, "keyb us") == 0) {
        keyboard_set_layout(LAYOUT_US);
        vga_print("Switched layout to US", 2);
    } 
    else {
        vga_print("Unknown command. Type 'help'.", 2);
    }
}

void tty_puts(const char *str);
void tty_putchar(char c);
void handle_command(const char *input_buffer);

void shell_run(void) {
    char buffer[128];
    uint8_t index = 0;

    tty_puts("\nLong-OS tty1\n> ");

    while (1) {
        char c = keyboard_getchar(); 
        if (!c) continue;

        if (c == '\n') {
            tty_putchar('\n');
            buffer[index] = '\0';
            
            if (index > 0) {
                handle_command(buffer);
            }
            
            index = 0;
            tty_puts("\n> ");
        } 
        else if (c == '\b') {
            if (index > 0) {
                index--;
                tty_putchar('\b');
            }
        } 
        else if (index < sizeof(buffer) - 1) {
            buffer[index++] = c;
            tty_putchar(c);
        }
    }
}