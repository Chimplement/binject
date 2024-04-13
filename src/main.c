#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <sys/mman.h>

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

    if (map_file(fd, PROT_READ, MAP_PRIVATE, (void**)&map, &map_size) == -1) {
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
                printf("ph[%i] offset: %p\n", i, (void*)program_headers[i].p_offset);
            }
        }
    }

    (void)munmap(map, map_size);
    return (0);
}