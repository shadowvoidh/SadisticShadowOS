bits 32
section .multiboot
    align 4
    dd 0x1BADB002              ; Magic Number do padrão Multiboot
    dd 0x00                    ; Flags
    dd - (0x1BADB002 + 0x00)   ; Checksum de validação

section .text
global _start
extern kernel_main

_start:
    cli                        ; Desabilita interrupções de hardware
    call kernel_main           ; Chama o ponto de entrada 
    hlt                        ; Trava a CPU caso a função C++ retorne


    MBALIGN     equ  1 << 0                   ; Alinha módulos carregados em páginas
MEMINFO     equ  1 << 1                   ; Fornece mapa de memória
VIDMOD      equ  1 << 2                   ; SOLICITA MODO GRÁFICO AO BOOTLOADER
FLAGS       equ  MBALIGN | MEMINFO | VIDMOD
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    
    ; Campos de endereço (não usados quando compilado em ELF)
    dd 0, 0, 0, 0, 0
    
    ; Configurações do Modo de Vídeo VBE
    dd 0                          ; 0 = Modo Gráfico (Linear Framebuffer)
    dd 1024                       ; Largura (Width)
    dd 768                        ; Altura (Height)
    dd 32                         ; BPP (Bits Per Pixel)

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB de pilha
stack_top:

section .text
global _start:function (_start.end - _start)
extern kernel_main

_start:
    mov esp, stack_top

    ; O GRUB passa um ponteiro para a estrutura Multiboot no registrador EBX
    push ebx
    call kernel_main

    cli
.hang:
    hlt
    jmp .hang
.end:

section .note.GNU-stack noalloc noexec nowrite progbits