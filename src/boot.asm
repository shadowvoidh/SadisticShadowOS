bits 32

; --- Configuração do Cabeçalho Multiboot ---
MBALIGN     equ 1 << 0                   ; Alinha módulos carregados em páginas
MEMINFO     equ 1 << 1                   ; Fornece mapa de memória
VIDMOD      equ 1 << 2                   ; Solicita modo gráfico ao bootloader
FLAGS       equ MBALIGN | MEMINFO | VIDMOD
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    
    ; Campos de endereço (não usados quando compilado em formato ELF)
    dd 0, 0, 0, 0, 0
    
    ; Configurações do Modo de Vídeo VBE
    dd 0                         ; 0 = Modo Gráfico (Linear Framebuffer)
    dd 1024                      ; Largura (Width)
    dd 768                       ; Altura (Height)
    dd 32                        ; BPP (Bits Per Pixel)

; --- Pilha de Memória (Stack) ---
section .bss
align 16
stack_bottom:
    resb 16384                   ; Reservar 16 KB de pilha
stack_top:

; --- Código Executável ---
section .text
global _start
extern kernel_main

_start:
    ; Configura a pilha de execução
    mov esp, stack_top

    ; O GRUB passa o ponteiro da estrutura Multiboot em EBX.
    ; Colocamos no registrador para ser lido como argumento no C++:
    push ebx

    ; Chama o kernel em C++
    call kernel_main

    ; Caso o kernel_main retorne, desabilita interrupções e trava a CPU
    cli
.hang:
    hlt
    jmp .hang