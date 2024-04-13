#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <sys/mman.h>

int main(int argc, char* argv[]) {
    if (argc < 2) return (1);

    Elf64_Ehdr *elf_header;

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        return (1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        (void)close(fd);
        return (1);
    }

    unsigned char* map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    (void)close(fd);
    if (map == NULL) {
        return (1);
    }

    if (file_size >= 64 && !memcmp(map, "\x7f""ELF", 4)) {
        elf_header = (Elf64_Ehdr*)map;
        printf("entry: %p\n", (void*)elf_header->e_entry);
    }

    (void)munmap(map, file_size);
    return (0);
}