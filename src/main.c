#include <stdint.h>
#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <sys/mman.h>

#include "min_max.h"
#include "mem_search.h"
#include "file_map.h"
#include "elf_map.h"

unsigned char shell_code[] = {
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

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        return (1);
    }

    unsigned char* map;
    size_t map_size;
    if (map_file(fd, PROT_READ | PROT_WRITE, MAP_PRIVATE, (void**)&map, &map_size) == -1) {
        close(fd);
        return (1);
    }
    close(fd);

    elf64_map_t elf_map = map_elf64(map, map_size);
    if (elf_map.start == NULL) {
        (void)munmap(map, map_size);
        return (1);
    }

    printf("entry: %p\n", (void*)elf_map.header->e_entry);

    Elf64_Phdr* executable_header = find_program_header(elf_map, PT_LOAD, PF_R | PF_X);
    if (executable_header == NULL) {
        (void)munmap(map, map_size);
        return (1);
    }

    uint8_t* code_cave = (uint8_t*)find_code_cave(
        elf_map.start + executable_header->p_offset,
        sizeof(shell_code),
        executable_header->p_filesz + sizeof(shell_code) + 16,
        8
    );

    if (code_cave == NULL) {
        (void)munmap(map, map_size);
        return (1);
    }

    printf("code_cave: %p\n", (void*)(code_cave - elf_map.start));

    int offset = elf_map.header->e_entry - (code_cave - elf_map.start) - 33;
    printf("jmp offset: %x (%i)\n", offset, offset);

    memcpy(shell_code + 29, &offset, 4);
    memcpy(code_cave, shell_code, sizeof(shell_code));

    executable_header->p_filesz = umax(
        executable_header->p_filesz,
        (code_cave - elf_map.start) + sizeof(shell_code) - executable_header->p_offset
    );
    executable_header->p_memsz = umax(
        executable_header->p_memsz,
        executable_header->p_filesz
    );
    elf_map.header->e_entry = code_cave - elf_map.start;

    fd = open("test", O_CREAT | O_WRONLY, 0744);
    if (fd != -1)
    {
        write(fd, map, map_size);
        close(fd);
    }

    (void)munmap(map, map_size);
    return (0);
}