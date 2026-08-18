#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "lib/string.h"

#define BUFFER_SIZE 128

// External function prototypes implemented in kernel/sys/commands/
void cmd_help(const char *args);
void cmd_fetch(const char *args);
void cmd_meminfo(const char *args);
void cmd_wall(const char *args);

typedef void (*cmd_fn_t)(const char *args);

typedef struct {
    const char *name;
    cmd_fn_t handler;
} command_t;

static const command_t cmd_table[] = {
    {"help",    cmd_help},
    {"fetch",   cmd_fetch},
    {"meminfo", cmd_meminfo},
    {"wall",    cmd_wall}
};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(command_t))

static void dispatch_command(const char *input_buffer) {
    if (input_buffer[0] == '\0') return;

    char cmd_name[32];
    size_t i = 0;

    // Parse command name
    while (input_buffer[i] != ' ' && input_buffer[i] != '\0' && i < 31) {
        cmd_name[i] = input_buffer[i];
        i++;
    }
    cmd_name[i] = '\0';

    const char *args = (input_buffer[i] == ' ') ? &input_buffer[i + 1] : "";

  
    for (size_t j = 0; j < CMD_COUNT; j++) {
        if (strcmp(cmd_name, cmd_table[j].name) == 0) {
            cmd_table[j].handler(args);
            return;
        }
    }

    tty_puts("Unknown command. Type 'help'.\n");
}

static void print_prompt(void) {
    tty_puts("longos@tty");
    tty_putchar('1' + tty_get_active());
    tty_puts("> ");
}

void shell_run(void) {
    char buffer[BUFFER_SIZE];
    uint8_t index = 0;

    print_prompt();

    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            tty_putchar('\n');
            buffer[index] = '\0';

            if (index > 0) {
                dispatch_command(buffer);
            }

            index = 0;
            print_prompt();
        } 
        else if (c == '\b') {
            if (index > 0) {
                index--;
                tty_putchar('\b');
            }
        } 
        else if (index < BUFFER_SIZE - 1) {
            buffer[index++] = c;
            tty_putchar(c);
        }
    }
}