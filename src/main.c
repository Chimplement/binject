#include <elf.h>

#include <fcntl.h>
#include <unistd.h>

int main() {
    unsigned char program[] = {
        0xb8, 0x3c, 0x00, 0x00, 0x00,   // mov 0x3c, eax
        0xbf, 0x00, 0x00, 0x00, 0x00,   // mov 0x0, edi
        0x0f, 0x05                      // syscall
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

    int fd = open("test.out", O_WRONLY | O_CREAT, 0744);
    if (fd == -1)
        return (0);

    (void)write(fd, &elf_header, sizeof(elf_header));
    (void)write(fd, &program_header, sizeof(program_header));
    (void)write(fd, &program, sizeof(program));

    (void)close(fd);

    return (0);
}