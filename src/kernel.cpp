// ---------------------------------------------------------------------------
// Funções Úteis de Porta I/O (Assembly embutido)
// ---------------------------------------------------------------------------

// Lê um byte de uma porta I/O do hardware
inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Escreve um byte em uma porta I/O do hardware
inline void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// ---------------------------------------------------------------------------
// Tabela de Scancodes do Teclado (Set 1) para ASCII
// ---------------------------------------------------------------------------
const char kbd_us[128] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

// ---------------------------------------------------------------------------
// Ponto de Entrada do Kernel
// ---------------------------------------------------------------------------
extern "C" void kernel_main() {
    // Ponteiro para o buffer de memória VGA em modo texto (0xB8000)
    volatile char* vga_buffer = reinterpret_cast<volatile char*>(0xB8000);
    int cursor = 0;

    // 1. Limpa a tela inteira (80 colunas x 25 linhas)
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga_buffer[i] = ' ';
        vga_buffer[i + 1] = 0x0F; // Texto branco em fundo preto
    }

    // 2. Exibe o título do sistema
    const char* title = "Sadistic Shadow OS esta ativo !!!";
    for (int i = 0; title[i] != '\0'; ++i) {
        vga_buffer[cursor * 2]     = title[i];
        vga_buffer[cursor * 2 + 1] = 0x0C; // Vermelho Claro
        cursor++;
    }

    // Pula para a próxima linha
    cursor = 80;

    // 3. Exibe a mensagem de prompt
    const char* prompt = "Digite algo: ";
    for (int i = 0; prompt[i] != '\0'; ++i) {
        vga_buffer[cursor * 2]     = prompt[i];
        vga_buffer[cursor * 2 + 1] = 0x0A; // Verde Claro
        cursor++;
    }

    // 4. Loop Infinito: Lê o teclado interativamente
    while (true) {
        // Verifica se há dados na porta de status do controlador de teclado (0x64)
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60); // Lê o scancode da porta 0x60
            
            // Verifica se a tecla foi PRESSIONADA (bit 0x80 = solta)
            if (!(scancode & 0x80)) {
                char letter = kbd_us[scancode];
                if (letter != 0) {
                    vga_buffer[cursor * 2]     = letter;
                    vga_buffer[cursor * 2 + 1] = 0x0F; // Texto branco
                    cursor++;
                }
            }
        }
    }
}