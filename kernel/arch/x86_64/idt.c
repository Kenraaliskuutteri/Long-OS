#include <stdint.h>
#include "kernel/idt.h"

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtr;

int idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base  = (uint64_t)&idt;

    __asm__ __volatile__ ("lidt %0" : : "m"(idtr));

    return 0;
}