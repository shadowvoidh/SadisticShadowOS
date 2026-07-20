all:
	mkdir -p build
	nasm -f elf32 src/boot.asm -o build/boot.o
	g++ -m32 -c src/kernel.cpp -o build/kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
	ld -m elf_i386 -T src/linker.ld build/boot.o build/kernel.o -o build/SSos.bin
	cp build/SSos.bin iso/boot/SSos.bin
	grub-mkrescue -o src/SSos.iso iso

clean:
	rm -rf build iso/boot/myos.bin