bits 32

MBALIGN     equ 1 << 0
MEMINFO     equ 1 << 1
FLAGS       equ MBALIGN | MEMINFO
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB de pilha
stack_top:

section .text
global _start
global load_idt
global keyboard_handler_asm
extern kernel_main
extern keyboard_handler_main

_start:
    mov esp, stack_top
    push ebx
    call kernel_main

    cli
.hang:
    hlt
    jmp .hang


load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    sti ; Ativa interrupções
    ret


keyboard_handler_asm:
    pusha
    call keyboard_handler_main
    popa
    iretd