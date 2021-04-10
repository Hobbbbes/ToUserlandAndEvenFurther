[bits 64]
init_stack:
    mov rsp,rsi ; Address of Stack in rsi (second argument)
    ;Bootinfo* in rdi first argument
    mov rbp, rsi
    call main

GLOBAL init_stack
EXTERN main