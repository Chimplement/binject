#include <elf.h>

#include <fcntl.h>
#include <unistd.h>

int main() {
    unsigned char program[] = {                                     // 0x400078:
        0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,                   // mov rax, 1
        0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00,                   // mov rdi, 1
        0x48, 0xbe, 0xA9, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rsi,0x4000A9
        0x48, 0xC7, 0xC2, 0x0E, 0x00, 0x00, 0x00,                   // mov rdx, 14
        0x0F, 0x05,                                                 // syscall
        0x48, 0xC7, 0xC0, 0x3C, 0x00, 0x00, 0x00,                   // mov rax, 60
        0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00,                   // mov rdi, 0
        0x0F, 0x05,                                                 // syscall
        // 0x4000A9:
        'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n',
    };

    Elf64_Ehdr elf_header = {
        .e_ident = {
            0x7f, 'E', 'L', 'F',
            ELFCLASS64,
            ELFDATA2LSB,
            EV_CURRENT,
            ELFOSABI_SYSV,
            0, 0, 0, 0, 0, 0, 0, 0
        },
        .e_type = ET_EXEC,
        .e_machine = EM_X86_64,
        .e_version = EV_CURRENT,
        .e_entry = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr),
        .e_phoff = sizeof(Elf64_Ehdr),
        .e_shoff = 0x0,
        .e_flags = 0x0,
        .e_ehsize = sizeof(Elf64_Ehdr),
        .e_phentsize = sizeof(Elf64_Phdr),
        .e_phnum = 1,
        .e_shentsize = sizeof(Elf64_Shdr),
        .e_shnum = 0,
        .e_shstrndx = SHN_UNDEF,
    };
    Elf64_Phdr program_header = {
        .p_type = PT_LOAD,
        .p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr),
        .p_vaddr = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr),
        .p_paddr = 0x0,
        .p_filesz = sizeof(program),
        .p_memsz = sizeof(program),
        .p_flags = PF_X | PF_R,
        .p_align = 0x1000,
    };

    int fd = open("hello_world.out", O_WRONLY | O_CREAT, 0744);
    if (fd == -1)
        return (0);

    (void)write(fd, &elf_header, sizeof(elf_header));
    (void)write(fd, &program_header, sizeof(program_header));
    (void)write(fd, &program, sizeof(program));

    (void)close(fd);

    return (0);
}