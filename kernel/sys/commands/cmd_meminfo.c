#include "drivers/vga.h"
#include <stdint.h>

#define BOOT_MEM_INFO_ADDR ((volatile uint16_t *)0x7000)

static uint64_t sys_get_total_ram_kb(void) {
    uint16_t ram_kb = *BOOT_MEM_INFO_ADDR;
    return (uint64_t)ram_kb;
}

static void print_uint(uint64_t val) {
    char buf[21];
    int i = 0;

    if (val == 0) {
        tty_putchar('0');
        return;
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i > 0) {
        tty_putchar(buf[--i]);
    }
}

void cmd_meminfo(const char *args) {
    (void)args;

    uint64_t ram_kb = sys_get_total_ram_kb();

    if (ram_kb == 0) {
        tty_puts("Memory detection failed or not set by bootloader.\n");
        return;
    }

    uint64_t ram_mb = ram_kb / 1024;

    tty_puts("Physical Memory Information:\n");
    tty_puts("  Detected RAM: ");
    print_uint(ram_mb);
    tty_puts(" MB (");
    print_uint(ram_kb);
    tty_puts(" KB)\n");
}