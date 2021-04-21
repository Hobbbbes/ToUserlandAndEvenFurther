[bits 64]

LoadGDT:
    lgdt [rdi]
    mov ax, 0x10 ; Offset of KernelData Entry
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi
    mov rax, 0x08 ; Offset of KernelCode Entry
    push rax
    push rdi
    retfq ; Far return to set code segment

GLOBAL LoadGDT: