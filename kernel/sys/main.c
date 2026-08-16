#include "drivers/vga.h"
#include "../../include/kernel/gdt.h"
#include "kernel/idt.h"

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