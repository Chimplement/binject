#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <sys/mman.h>

#include "min_max.h"
#include "mem_search.h"
#include "file_mapping.h"

unsigned char program[] = {
    0x52,                                       // push   %rdx
    0xb8, 0x01, 0x00, 0x00, 0x00,               // mov    $0x1,%eax
    0xbf, 0x01, 0x00, 0x00, 0x00,               // mov    $0x1,%edi
    0x48, 0x8d, 0x35, 0x11, 0x00, 0x00, 0x00,   // lea    0x11(%rip),%rsi
    0xba, 0x0e, 0x00, 0x00, 0x00,               // mov    $0xe,%edx
    0x0f, 0x05,                                 // syscall
    0x5a,                                       // pop    %rdx
    0x48, 0x8d, 0x05, 0xff, 0xff, 0xff, 0xff,   // lea    -0x1(%rip),%rax
    0xff, 0xe0,                                 // jmp    *%rax
    'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n',
};

int main(int argc, char* argv[]) {
    if (argc < 2) return (1);

    unsigned char* map;
    size_t map_size;
    Elf64_Ehdr* elf_header;
    Elf64_Phdr* program_headers;
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        return (1);
    }

    if (map_file(fd, PROT_READ | PROT_WRITE, MAP_PRIVATE, (void**)&map, &map_size) == -1) {
        close(fd);
        return (1);
    }
    close(fd);

    if (map_size >= 64 && !memcmp(map, "\x7f""ELF", 4) && ((char*)map)[4] == 0x2) {
        elf_header = (Elf64_Ehdr*)map;
        printf("entry: %p\n", (void*)elf_header->e_entry);
        if (elf_header->e_phentsize * elf_header->e_phnum + elf_header->e_phoff <= map_size) {
            program_headers = (Elf64_Phdr*)(map + elf_header->e_phoff);
            for (int i = 0; i < elf_header->e_phnum; i++) {
                if (program_headers[i].p_type == PT_LOAD && \
                        program_headers[i].p_flags & PF_R && \
                        program_headers[i].p_flags & PF_X) {
                    void* code_cave = (char*)find_code_cave(
                        (char*)map + program_headers[i].p_offset,
                        sizeof(program) + 100,
                        program_headers[i].p_filesz + sizeof(program) + 100
                    ) + 50;
                    if (code_cave != NULL)
                    {
                        printf("code_cave: %p\n", (void*)((char*)code_cave - (char*)map));
                        int offset = elf_header->e_entry - ((char*)code_cave - (char*)map) - 33;
                        printf("jmp offset: %x (%i)\n", offset, offset);
                        memcpy(program + 29, &offset, 4);
                        memcpy(code_cave, program, sizeof(program));
                        program_headers[i].p_filesz += sizeof(program) + 100;
                        program_headers[i].p_memsz += sizeof(program) + 100;
                        elf_header->e_entry = (char*)code_cave - (char*)map;
                        break;
                    }
                }
            }
        }
    }

    fd = open("test", O_CREAT | O_WRONLY, 0744);
    if (fd != -1)
    {
        write(fd, map, map_size);
        close(fd);
    }

    (void)munmap(map, map_size);
    return (0);
}