BOOTLOADER = build/bootloader.bin
KERNEL     = build/kernel.bin
ISO_IMAGE  = build/longos-live.iso

CC      = gcc
CFLAGS  = -ffreestanding -m64 -O2 -Wall -Wextra -Iinclude
NASM    = nasm

all: $(BOOTLOADER) $(KERNEL)


$(BOOTLOADER): boot/boot.asm
	@mkdir -p build
	$(NASM) -f bin boot/boot.asm -o $(BOOTLOADER)

$(KERNEL): kernel/sys/main.c kernel/arch/x86_64/idt.c kernel/arch/x86_64/gdt.c kernel/drivers/display/vga.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c kernel/sys/main.c -o build/main.o
	$(CC) $(CFLAGS) -c kernel/arch/x86_64/idt.c -o build/idt.o
	$(CC) $(CFLAGS) -c kernel/arch/x86_64/gdt.c -o build/gdt.o
	$(CC) $(CFLAGS) -c kernel/drivers/display/vga.c -o build/vga.o
	# Link kernel binaries together
	ld -m elf_x86_64 -T x86_64-kernel.ld build/main.o build/idt.o build/gdt.o build/vga.o -o $(KERNEL)


iso: all
	@mkdir -p iso_root/boot
	cp $(BOOTLOADER) iso_root/boot/
	cp $(KERNEL) iso_root/boot/
	xorriso -as mkisofs -b boot/bootloader.bin -no-emul-boot -boot-load-size 4 -o $(ISO_IMAGE) iso_root

clean:
	rm -rf build iso_root