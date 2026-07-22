// ---------------------------------------------------------------------------
// Funções Úteis de Porta I/O (Assembly embutido)
// ---------------------------------------------------------------------------

// Lê um byte de uma porta I/O do hardware
inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Escreve um byte ou palavra em uma porta I/O do hardware
inline void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

inline void outw(unsigned short port, unsigned short data) {
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(port));
}

// ---------------------------------------------------------------------------
// Desligamento do Sistema (Shutdown via ACPI / QEMU / Bochs)
// ---------------------------------------------------------------------------
void shutdown() {
    // Tenta desligar via porta do QEMU/VirtualBox (ACPI)
    outw(0x604, 0x2000);
    // Tenta desligar via porta alternativa do Bochs/QEMU
    outw(0xB004, 0x2000);

    // Se a máquina virtual não desligar, trava a CPU em loop seguro
    asm volatile ("cli");
    while (true) {
        asm volatile ("hlt");
    }
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
// Comparador de Strings Simples (sem std::strcmp)
// ---------------------------------------------------------------------------
bool strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return str1[i] == str2[i];
}

// ---------------------------------------------------------------------------
// Ponto de Entrada do Kernel
// ---------------------------------------------------------------------------
extern "C" void kernel_main() {
    volatile char* vga_buffer = reinterpret_cast<volatile char*>(0xB8000);
    
    int cursor = 0;
    char input_buffer[80]; // Guarda o comando digitado na linha atual
    int buffer_idx = 0;
    int prompt_start_cursor = 0; // Posição onde o usuário começa a digitar

    // 1. Limpa a tela
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga_buffer[i] = ' ';
        vga_buffer[i + 1] = 0x0F;
    }

    // 2. Exibe o Título
    const char* title = "Sadistic Shadow OS esta ativo !!!";
    for (int i = 0; title[i] != '\0'; ++i) {
        vga_buffer[cursor * 2]     = title[i];
        vga_buffer[cursor * 2 + 1] = 0x0C; // Vermelho
        cursor++;
    }

    // Pula para a próxima linha
    cursor = 80;

    // 3. Exibe o Prompt Inicial
    const char* prompt = "shadow> ";
    for (int i = 0; prompt[i] != '\0'; ++i) {
        vga_buffer[cursor * 2]     = prompt[i];
        vga_buffer[cursor * 2 + 1] = 0x0A; // Verde
        cursor++;
    }
    prompt_start_cursor = cursor;

    // 4. Loop do Teclado
    while (true) {
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);
            
            // Tecla Pressionada (não solta)
            if (!(scancode & 0x80)) {
                char letter = kbd_us[scancode];

                // --- TRATAMENTO DO ENTER ---
                if (letter == '\n') {
                    input_buffer[buffer_idx] = '\0'; // Finaliza a string do comando

                    // Verifica se o comando foi de desligar
                    if (strcmp(input_buffer, "exit") || strcmp(input_buffer, "shutdown")) {
                        shutdown();
                    }

                    // Pula para o início da próxima linha na tela VGA (múltiplo de 80)
                    cursor = ((cursor / 80) + 1) * 80;
                    
                    // Exibe o prompt na nova linha
                    for (int i = 0; prompt[i] != '\0'; ++i) {
                        vga_buffer[cursor * 2]     = prompt[i];
                        vga_buffer[cursor * 2 + 1] = 0x0A;
                        cursor++;
                    }

                    // Reseta o buffer do comando
                    buffer_idx = 0;
                    prompt_start_cursor = cursor;
                }
                // --- TRATAMENTO DO BACKSPACE ---
                else if (letter == '\b') {
                    // Impede apagar o prompt "shadow> "
                    if (cursor > prompt_start_cursor && buffer_idx > 0) {
                        cursor--;
                        buffer_idx--;
                        vga_buffer[cursor * 2]     = ' ';
                        vga_buffer[cursor * 2 + 1] = 0x0F;
                    }
                }
                // --- CARACTERE NORMAL / ESPAÇO ---
                else if (letter != 0) {
                    if (buffer_idx < 79) { // Limite da linha
                        input_buffer[buffer_idx++] = letter;
                        vga_buffer[cursor * 2]     = letter;
                        vga_buffer[cursor * 2 + 1] = 0x0F; // Texto branco
                        cursor++;
                    }
                }
            }
        }
    }
}