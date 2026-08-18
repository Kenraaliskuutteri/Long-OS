#include <stdint.h>
#include "kernel/arch/x86_64/io.h"

static volatile uint64_t ticks = 0;

void timer_phase(int hz) {
    int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_handler(void) {
    ticks++;
}

uint64_t timer_get_ticks(void) {
    return ticks;
}

uint64_t timer_get_seconds(void) {
    return ticks / 100; 
}