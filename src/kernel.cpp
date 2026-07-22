// ---------------------------------------------------------------------------
// Funções Úteis de Porta I/O (Assembly embutido)
// ---------------------------------------------------------------------------
inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

inline void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

inline void outw(unsigned short port, unsigned short data) {
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(port));
}

// ---------------------------------------------------------------------------
// Desligamento do Sistema (Múltiplos Métodos de Hardware)
// ---------------------------------------------------------------------------
void shutdown() {
    // 1. Método QEMU (ACPI shutoff)
    outw(0x604, 0x2000);

    // 2. Método Bochs / Máquinas Virtuais Antigas
    outw(0xB004, 0x2000);

    // 3. Método VirtualBox / QEMU alternativo (ACPI PM1a_CNT)
    outw(0x4004, 0x3400);

    // 4. Reinício / Desligamento via Controlador de Teclado (PS/2)
    outb(0x64, 0xFE);

    // Se nenhum método fechar a VM, congela a CPU com segurança
    asm volatile ("cli");
    while (true) {
        asm volatile ("hlt");
    }
}

// ---------------------------------------------------------------------------
// Comparador de Strings Seguro
// ---------------------------------------------------------------------------
bool streql(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return false;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

// ---------------------------------------------------------------------------
// Estruturas do Multiboot 1
// ---------------------------------------------------------------------------
struct MultibootInfo {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;
    unsigned int mods_addr;
    unsigned int syms[4];
    unsigned int mmap_length;
    unsigned int mmap_addr;
    unsigned int drives_length;
    unsigned int drives_addr;
    unsigned int config_table;
    unsigned int boot_loader_name;
    unsigned int apm_table;
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    
    // Framebuffer Info (Flags bit 12)
    unsigned long long framebuffer_addr;
    unsigned int framebuffer_pitch;
    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    unsigned char framebuffer_bpp;
    unsigned char framebuffer_type;
};

// ---------------------------------------------------------------------------
// Fonte Bitmap 8x16 Básica (Caracteres Padrão)
// ---------------------------------------------------------------------------
// Tabela compatível com C++11/C++14/C++17 (sem índices estilo C)
static const unsigned char font8x16_basic[128][16] = {
    // 0..31: Caracteres de controle não imprimíveis
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    
    // 32: Espaço ' '
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 33: '!'
    {0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 34..61
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    
    // 62: '>'
    {0x00,0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 63..64
    {0}, {0},

    // 65: 'A'
    {0x00,0x18,0x3C,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00},
    // 66: 'B'
    {0x00,0x7C,0x66,0x66,0x7C,0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 67: 'C'
    {0x00,0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 68: 'D'
    {0x00,0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 69: 'E'
    {0x00,0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 70: 'F'
    {0x00,0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 71: 'G'
    {0x00,0x3C,0x66,0x60,0x6E,0x66,0x66,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 72: 'H'
    {0x00,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 73: 'I'
    {0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 74..75
    {0}, {0},
    // 76: 'L'
    {0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 77
    {0},
    // 78: 'O'
    {0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 79: 'P'
    {0x00,0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 80..81
    {0}, {0},
    // 82: 'R'
    {0x00,0x7C,0x66,0x66,0x7C,0x6C,0x66,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 83: 'S'
    {0x00,0x3C,0x66,0x30,0x1C,0x06,0x66,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 84: 'T'
    {0x00,0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 85..96
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},

    // 97: 'a'
    {0x00,0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 98..99
    {0}, {0},
    // 100: 'd'
    {0x00,0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 101: 'e'
    {0x00,0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 102..103
    {0}, {0},
    // 104: 'h'
    {0x00,0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 105: 'i'
    {0x00,0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 106..110
    {0}, {0}, {0}, {0}, {0},
    // 111: 'o'
    {0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 112..114
    {0}, {0}, {0},
    // 115: 's'
    {0x00,0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 116: 't'
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 117..127
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
};

// ---------------------------------------------------------------------------
// Variáveis Globais de Vídeo
// ---------------------------------------------------------------------------
volatile unsigned int* fb_addr = nullptr;
unsigned int fb_width = 1024;
unsigned int fb_height = 768;
unsigned int fb_pitch = 1024 * 4;

// ---------------------------------------------------------------------------
// Desenhar Primitivas Gráficas
// ---------------------------------------------------------------------------
void draw_pixel(int x, int y, unsigned int color) {
    if (x >= 0 && x < (int)fb_width && y >= 0 && y < (int)fb_height && fb_addr != nullptr) {
        fb_addr[y * (fb_pitch / 4) + x] = color;
    }
}

void draw_rect(int x, int y, int w, int h, unsigned int color) {
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            draw_pixel(j, i, color);
        }
    }
}

// Renderiza um caractere ASCII na posição (x, y)
void draw_char(char c, int x, int y, unsigned int fg_color) {
    unsigned char uc = (unsigned char)c;
    if (uc >= 128) uc = ' ';
    
    const unsigned char* glyph = font8x16_basic[uc];
    for (int cy = 0; cy < 16; ++cy) {
        unsigned char row = glyph[cy];
        for (int cx = 0; cx < 8; ++cx) {
            if (row & (1 << (7 - cx))) {
                draw_pixel(x + cx, y + cy, fg_color);
            }
        }
    }
}

// Renderiza uma string completa
void draw_string(const char* str, int x, int y, unsigned int fg_color) {
    int cur_x = x;
    for (int i = 0; str[i] != '\0'; ++i) {
        draw_char(str[i], cur_x, y, fg_color);
        cur_x += 8;
    }
}

// Desenha uma Janela com Barra de Título
void draw_window(int x, int y, int w, int h, const char* title, unsigned int bg_color, unsigned int title_color) {
    // Fundo da janela
    draw_rect(x, y, w, h, bg_color);
    // Barra de título
    draw_rect(x, y, w, 28, title_color);
    // Texto do título
    draw_string(title, x + 10, y + 6, 0x00FFFFFF);
    // Botão de Fechar (X)
    draw_rect(x + w - 22, y + 4, 18, 18, 0x00FF4444);
}

// ---------------------------------------------------------------------------
// Ponto de Entrada do Kernel
// ---------------------------------------------------------------------------
extern "C" void kernel_main(MultibootInfo* mb_info) {
    // 1. Obter endereço do Framebuffer retornado pelo GRUB
    if (mb_info != nullptr && (mb_info->flags & (1 << 12))) {
        fb_addr = reinterpret_cast<volatile unsigned int*>(mb_info->framebuffer_addr);
        fb_width = mb_info->framebuffer_width;
        fb_height = mb_info->framebuffer_height;
        fb_pitch = mb_info->framebuffer_pitch;
    } else {
        // Fallback para endereço padrão do VBE QEMU se não fornecido
        fb_addr = reinterpret_cast<volatile unsigned int*>(0xFD000000);
    }

    // 2. Desenhar Área de Trabalho (Fundo Roxo Escuro)
    draw_rect(0, 0, fb_width, fb_height, 0x00181825);

    // 3. Desenhar Barra de Tarefas Inferior
    draw_rect(0, fb_height - 40, fb_width, 40, 0x0011111B);
    draw_string("Sadistic OS", 15, fb_height - 26, 0x00A6ADC8);

    // 4. Desenhar Janela Principal
    draw_window(150, 120, 500, 320, "Sadistic Shadow OS - GUI", 0x00313244, 0x0089B4FA);
    draw_string("Modo Grafico Ativo!", 180, 180, 0x00A6E3A1);
    draw_string("Resolucao: 1024x768 32bpp", 180, 210, 0x00CDD6F4);

    // 5. Desenhar Ponteiro de Mouse Simples (Triângulo amarelo)
    int mouse_x = 320;
    int mouse_y = 240;
    for (int i = 0; i < 12; ++i) {
        draw_rect(mouse_x, mouse_y + i, 12 - i, 1, 0x00F9E2AF);
    }

    // Loop do Kernel
    while (true) {
        asm volatile ("hlt");
    }
}