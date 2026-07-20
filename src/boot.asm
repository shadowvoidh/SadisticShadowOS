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