#ifndef VGA_H
#define VGA_H

#include "drivers/vga.h"
#include "../../include/kernel/gdt.h"
#include "kernel/idt.h"
#include "stdint.h"

void tty_init(void);
void tty_puts(const char *str);
void tty_putchar(char c);
void tty_switch(uint8_t tty_index);
uint8_t tty_get_active(void);

void kmain(void) {
    tty_init();
    tty_puts("Long-OS Kernel Starting...\n");

    if (gdt_init() == 0) {
        tty_puts("[OK] GDT Initialized\n");
    } else {
        tty_puts("[CRITICAL] GDT Initialization Failed\n");
    }

    if (idt_init() == 0) {
        tty_puts("[  OK  ] IDT Initialized\n");
    } else {
        tty_puts("[CRITICAL] IDT Initialization Failed\n");
    }

    tty_puts("Ready for memory management setup.\n");

    // Infinite halt loop, halting CPU to save power
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
#endif