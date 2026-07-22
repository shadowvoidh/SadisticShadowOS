
extern "C" void kernel_main() {
   
    volatile char* vga_buffer = reinterpret_cast<volatile char*>(0xB8000);

    const char* message = "Sadistic Shadow OS esta insano ativo !!!";
    
    
    const char color_attribute = 0x0F;

    
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga_buffer[i] = ' ';
        vga_buffer[i + 1] = color_attribute;
    }

   
    for (int i = 0; message[i] != '\0'; ++i) {
        vga_buffer[i * 2]     = message[i];      
        vga_buffer[i * 2 + 1] = color_attribute;  
    }

    
    while (true) {
        asm volatile("hlt");
    }
}

inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Escreve um byte em uma porta I/O
inline void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Tabela simples de Scancodes do Teclado (Set 1) para caracteres ASCII
const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

extern "C" void kernel_main() {
    volatile char* vga_buffer = reinterpret_cast<volatile char*>(0xB8000);
    int cursor = 0;

    // Limpa a tela
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga_buffer[i] = ' ';
        vga_buffer[i + 1] = 0x0F;
    }

    // Mensagem inicial
    const char* prompt = "Digite algo: ";
    for (int i = 0; prompt[i] != '\0'; ++i) {
        vga_buffer[cursor * 2] = prompt[i];
        vga_buffer[cursor * 2 + 1] = 0x0A; // Verde
        cursor++;
    }

    // Loop de leitura do teclado
    while (true) {
        // Verifica se há dados na porta de status do teclado (0x64)
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60); // Lê o caractere enviado
            
            // Se o bit mais significativo não estiver setado, a tecla foi pressionada (não solta)
            if (!(scancode & 0x80)) {
                char letter = kbd_us[scancode];
                if (letter != 0) {
                    vga_buffer[cursor * 2] = letter;
                    vga_buffer[cursor * 2 + 1] = 0x0F; // Texto branco
                    cursor++;
                }
            }
        }
    }
}