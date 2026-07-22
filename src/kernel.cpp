typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t           uintptr_t;

// --- COMUNICAÇÃO DE HARDWARE (PORTAS I/O) ---
extern "C" {
    void load_idt(void* idt_ptr);
    void keyboard_handler_asm();
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- TERMINAL VGA ---
volatile uint16_t* const VGA_BUFFER = (uint16_t*) 0xB8000;
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;
int terminal_row = 0;
int terminal_column = 0;
uint8_t terminal_color = 0x0F; 

void terminal_putc(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else if (c == '\b') { 
        if (terminal_column > 0) {
            terminal_column--;
            VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_column] = (uint16_t)' ' | ((uint16_t)terminal_color << 8);
        }
    } else {
        const int index = terminal_row * VGA_WIDTH + terminal_column;
        VGA_BUFFER[index] = (uint16_t)c | ((uint16_t)terminal_color << 8);
        terminal_column++;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT) {
        for (int y = 0; y < VGA_HEIGHT - 1; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
            }
        }
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | ((uint16_t)terminal_color << 8);
        }
        terminal_row = VGA_HEIGHT - 1;
    }
}

void terminal_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_putc(str[i]);
    }
}

// --- TABELA DE INTERRUPÇÕES (IDT) ---
struct IDTEntry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

IDTEntry idt[256];
IDTPtr idt_ptr;

void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// --- DRIVER DE TECLADO PS/2 ---
unsigned char kbd_layout[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',  0, ' '
};

extern "C" void keyboard_handler_main() {
    uint8_t scancode = inb(0x60);
    
    if (!(scancode & 0x80)) {
        char letter = kbd_layout[scancode];
        if (letter != 0) {
            terminal_putc(letter);
        }
    }
    outb(0x20, 0x20);
}

void init_idt() {
    idt_ptr.limit = (sizeof(IDTEntry) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFD); outb(0xA1, 0xFF); 
    // Configura a interrupção 
    set_idt_gate(33, (uint32_t)keyboard_handler_asm, 0x08, 0x8E);

    load_idt(&idt_ptr);
}

// --- PONTO DE ENTRADA DO KERNEL ---
extern "C" void kernel_main(void* multiboot_structure) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = (uint16_t)' ' | ((uint16_t)terminal_color << 8);
    }
    terminal_row = 0;
    terminal_column = 0;

    terminal_print("=========================================\n");
    terminal_print("       SISTEMA OPERACIONAL EM C++        \n");
    terminal_print("=========================================\n\n");
    terminal_print("[OK] Inicializando IDT e Teclado...\n");

    init_idt();

    terminal_print("[OK] Teclado pronto! Digite algo abaixo:\n\n> ");
}