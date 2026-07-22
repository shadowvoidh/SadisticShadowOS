# Sadistic Shadow OS 🌒 (Beta)

**Sadistic Shadow OS** é um sistema operacional x86 de 32 bits desenvolvido do zero em C++ (*Freestanding*) e Assembly, utilizando a especificação **Multiboot** e o bootloader **GRUB**.

---

## 🚀 Funcionalidades Atuais

- **Arquitetura 32-bit (x86)**: Execução direta em camada *bare-metal*.
- **Suporte Multiboot**: Compatível com o GRUB e boot direto via QEMU.
- **Interface Gráfica (GUI) Nativa**:
  - Resolução de vídeo em **1024x768**.
  - Ambiente gráfico customizado com barra de tarefas.
  - Janela de interface do utilizador ("Sadistic Shadow OS - GUI").
  - Renderização do ponteiro do cursor.

---

## 🛠️ Pré-requisitos & Ferramentas

Para compilar e testar o projeto, precisas das seguintes ferramentas instaladas no Linux/WSL:

- **GCC / G++** (com suporte a `-m32`)
- **NASM** (*Netwide Assembler*)
- **GNU Linker (`ld`)**
- **Utilitários do GRUB** (`grub-mkrescue`, `xorriso`, `mtools`)
- **QEMU** (`qemu-system-i386`)

No Ubuntu/Debian, instala com:
```bash
sudo apt update
sudo apt install build-essential g++-multilib nasm grub-pc-bin grub-common xorriso mtools qemu-system-x86