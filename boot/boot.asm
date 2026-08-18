; STAGE 1: Boot Sector (Loaded by BIOS at 0x7C00, exactly 512 bytes)
[BITS 16]
[ORG 0x7c00]

detect_memory:
    ; Query extended memory size (in KB) above 1MB via BIOS INT 15h, AH=88h
    mov ah, 0x88
    int 0x15
    jc .error               ; Carry flag set on error

    ; AX now contains extended memory in KB
    ; Add 1024 KB (1MB) to account for conventional base memory
    add ax, 1024            

    ; Store total KB result at fixed physical address 0x7000
    mov [0x7000], ax
    ret

.error:
    mov word [0x7000], 0    ; Store 0 if detection failed
    ret

CODE_OFFSET equ 0x8
DATA_OFFSET equ 0x10
CODE64_OFFSET equ 0x18
DATA64_OFFSET equ 0x20
KERNEL_PHYS_ADDR equ 0x10000   ; physical address where kernel runs
ORG_BASE equ 0x7c00

start:
    cli                        ; turn off interrupts during setup
    mov [boot_drive], dl        ; save drive number passed by BIOS
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti                        ; re-enable interrupts

    mov si, msg
    call print

    call load_stage2
    jmp STAGE2_LOAD_SEGMENT:STAGE2_LOAD_OFFSET

print:
    lodsb
    cmp al, 0
    je print_done
    mov ah, 0x0E
    int 0x10
    jmp print
print_done:
    ret

msg: db 'Now loading: Long-OS...', 13, 10, 0
err_msg: db 'Disk Read Error!', 0

STAGE2_LOAD_SEGMENT equ 0x0000
STAGE2_LOAD_OFFSET  equ 0x7e00   ; loaded right after boot sector in RAM
STAGE2_SECTOR_COUNT equ 40       ; read 40 sectors for stage 2 (code + GDT + page tables)
STAGE2_START_LBA    equ 1        ; sector 1 on disk

load_stage2:
    mov si, dap
    mov ah, 0x42                 ; extended BIOS disk read
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, err_msg
    call print
    cli
    hlt

align 4
dap:                            ; disk address packet
    db 0x10
    db 0
    dw STAGE2_SECTOR_COUNT
    dw STAGE2_LOAD_OFFSET
    dw STAGE2_LOAD_SEGMENT
    dq STAGE2_START_LBA

boot_drive: db 0

times 510 - ($-$$) db 0
dw 0xAA55                       ; boot sector signature (ends 1st stage)

; STAGE 2: 32-bit Protected Mode & 64-bit Long Mode Transition
STAGE2_START:

[BITS 16]
    call load_kernel            ; load kernel while still in real mode

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0 
    or al, 1                    ; set protected mode bit
    mov cr0, eax
    jmp CODE_OFFSET:PModeMain

KERNEL_LOAD_SEGMENT equ 0x1000   ; 0x1000:0x0000 = physical 0x10000
KERNEL_LOAD_OFFSET  equ 0x0000
KERNEL_SECTOR_COUNT equ 100      ; sectors to read for kernel
KERNEL_START_LBA    equ STAGE2_START_LBA + STAGE2_SECTOR_COUNT

load_kernel:
    mov si, kernel_dap
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    ret

align 4
kernel_dap:
    db 0x10
    db 0
    dw KERNEL_SECTOR_COUNT
    dw KERNEL_LOAD_OFFSET
    dw KERNEL_LOAD_SEGMENT
    dq KERNEL_START_LBA

[BITS 32]
PModeMain:
    mov ax, DATA_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    ; enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; zero out page tables (12 KiB total)
    mov edi, pml4_table
    mov cr3, edi
    xor eax, eax
    mov ecx, 3072
    rep stosd

    ; link page tables (PML4 -> PDPT -> PD)
    mov eax, pdpt_table
    or eax, 0b011               ; present + writeable
    mov [pml4_table], eax

    mov eax, page_directory
    or eax, 0b011               ; present + writeable
    mov [pdpt_table], eax

    ; identity map first 2MB using a huge page
    mov eax, 0x00000000
    or eax, 0b10000011          ; present + writeable + 2MB huge page
    mov [page_directory], eax

    ; enable PAE in CR4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set long mode bit in EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; turn on paging in CR0
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax

    lgdt [gdt_descriptor]

    ; jump to 64-bit code
    jmp CODE64_OFFSET:LongModeMain

[BITS 64]
LongModeMain:
    mov ax, DATA64_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x90000

    mov rax, KERNEL_PHYS_ADDR
    jmp rax

; GDT
align 8
gdt_start:
    dq 0x0000000000000000        ; null descriptor

gdt_code32:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data32:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_code64:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10011010b
    db 00100000b
    db 0x00

gdt_data64:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10010010b
    db 0x00
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; Page tables (page-aligned, 3 x 4KiB: PML4, PDPT, PD)
; align only pads to a multiple of the file offset, not the ORG-adjusted
; runtime address, so alignment is computed manually against ORG_BASE

times (4096 - (($-$$+ORG_BASE) % 4096)) % 4096 db 0
pml4_table:
    times 4096 db 0
pdpt_table:
    times 4096 db 0
page_directory:
    times 4096 db 0

times (STAGE2_SECTOR_COUNT * 512) - ($-STAGE2_START) db 0
