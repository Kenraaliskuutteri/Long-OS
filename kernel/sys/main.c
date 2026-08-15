#include "drivers/vga.h"
#include "../../include/kernel/gdt.h"
#include "kernel/idt.h"

void kmain(void) {
    vga_init();
    vga_puts("Long-OS Kernel Starting...\n");

if (gdt_init() == 0) {
    vga_puts("[OK] GDT Initialized\n");
} else{
    vga_puts("[CRITICAL] GDT Initialization Failed\n");
}

if (idt_init() == 0) {
    vga_puts("[  OK  ] IDT Initialized\n");
} else {
    vga_puts("[CRITICAL] IDT Initialization Failed\n");
    
    vga_puts("Ready for memory management setup.\n");

    // Infinite halt loop, halting CPU to save power
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
}