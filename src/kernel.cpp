
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