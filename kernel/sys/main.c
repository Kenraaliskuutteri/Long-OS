#include "drivers/vga.h"
#include "../../include/kernel/gdt.h"
#include "kernel/idt.h"

void kmain(void) {
    vga_init();
    vga_puts("Long-OS Kernel Starting...\n");

    gdt_init();
    vga_puts("[OK] GDT Initialized\n");

    idt_init();
    vga_puts("[OK] IDT Initialized\n");

    vga_puts("Ready for memory management setup.\n");

    // Infinite halt loop, halting CPU to save power
    while (1) {
        __asm__ __volatile__("hlt");
    }
}