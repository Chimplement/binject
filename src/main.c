#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <sys/mman.h>

#include "min_max.h"

off_t get_file_size(int fd) {
    off_t original_offset = lseek(fd, 0, SEEK_CUR);
    if (original_offset == (off_t)-1) {
        return (-1);
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    (void)lseek(fd, original_offset, SEEK_SET);
    if (file_size == (off_t)-1) {
        return (-1);
    }
    return (file_size);
}

int map_file(int fd, int prot, int flags, void** map, size_t* map_size) {
    off_t file_size = get_file_size(fd);
    if (file_size == (off_t)-1) {
        return (-1);
    }

    *map_size = file_size;
    *map = mmap(NULL, *map_size, prot, flags, fd, 0);
    if (*map == MAP_FAILED) {
        return (-1);
    }
    return (0);
}

size_t mem_count(void* start, char byte, size_t max_repeat) {
    size_t repeat = 0;
    while (repeat < max_repeat && ((char*)start)[repeat] == byte) {
        repeat++;
    }
    return (repeat);
}

void* find_code_cave(void* start, size_t min_cave_size, size_t search_length) {
    size_t offset = 0;
    while (search_length - offset >= min_cave_size) {
        size_t zero_count = mem_count((char*)start + offset, 0, min_cave_size);
        if (zero_count >= min_cave_size) {
            return ((char*)start + offset);
        }
        offset += umax(zero_count, 1);
    }
    return (NULL);
}

unsigned char program[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00,               // mov    $0x1,%eax
    0xbf, 0x01, 0x00, 0x00, 0x00,               // mov    $0x1,%edi
    0x48, 0x8d, 0x35, 0x11, 0x00, 0x00, 0x00,   // lea    0x11(%rip),%rsi
    0xba, 0x0e, 0x00, 0x00, 0x00,               // mov    $0xe,%edx
    0x0f, 0x05,                                 // syscall
    0xb8, 0x3c, 0x00, 0x00, 0x00,               // mov    $0x3c,%eax
    0x48, 0x31, 0xff,                           // xor    %rdi,%rdi
    0x0f, 0x05,
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
                    void* code_cave = find_code_cave(
                        (char*)map + program_headers[i].p_offset,
                        sizeof(program),
                        program_headers[i].p_filesz + sizeof(program)
                    );
                    if (code_cave != NULL)
                    {
                        printf("code_cave: %p\n", (void*)((char*)code_cave - (char*)map));
                        memcpy(code_cave, program, sizeof(program));
                        program_headers[i].p_filesz += sizeof(program);
                        program_headers[i].p_memsz += sizeof(program);
                        elf_header->e_entry = (char*)code_cave - (char*)map;
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