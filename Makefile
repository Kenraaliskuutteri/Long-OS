BOOTLOADER = build/bootloader.bin
KERNEL     = build/kernel.bin
ISO_IMAGE  = build/longos-live.iso

CC      = gcc
CFLAGS  = -ffreestanding -m64 -O2 -Wall -Wextra -Ikernel
NASM    = nasm

all: $(BOOTLOADER) $(KERNEL)


$(BOOTLOADER): boot/stage1.asm
	@mkdir -p build
	$(NASM) -f bin boot/boot.asm -o $(BOOTLOADER)

$(KERNEL): kernel/main.c kernel/arch/x86_64/idt.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c kernel/main.c -o build/main.o
	$(CC) $(CFLAGS) -c kernel/arch/x86_64/idt.c -o build/idt.o
	# Link kernel binaries together
	ld -m elf_x86_64 -T linker.ld build/main.o build/idt.o -o $(KERNEL)


iso: all
	@mkdir -p iso_root/boot
	cp $(BOOTLOADER) iso_root/boot/
	cp $(KERNEL) iso_root/boot/
	xorriso -as mkisofs -b boot/bootloader.bin -no-emul-boot -boot-load-size 4 -o $(ISO_IMAGE) iso_root

clean:
	rm -rf build iso_root
