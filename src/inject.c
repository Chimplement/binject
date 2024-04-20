#include "elf_map.h"

#include "elf_map.h"

#include "inject.h"

#define CODE_CAVE_PADDING 8

int inject32(void* map, size_t map_size, void* payload, size_t payload_size) {
    return (-1);
}

static uint8_t payload_prefix64[] = {
    0x48, 0x8d, 0x05, 0xff, 0xff, 0xff, 0xff,   // lea  -0x1(%rip),%rax
    0x50,                                       // push %rax
};

int inject64(void* map, size_t map_size, void* payload, size_t payload_size) {
    elf64_map_t elf_map = map_elf64(map, map_size);
    if (elf_map.start == NULL) {
        return (-1);
    }

    Elf64_Phdr* executable_header = find_program_header(elf_map, PT_LOAD, PF_R | PF_X);
    if (executable_header == NULL) {
        return (-1);
    }

    uint8_t* code_cave = (uint8_t*)find_code_cave(
        elf_map.start + executable_header->p_offset,
        sizeof(payload_prefix64) + payload_size,
        umin(
            sizeof(payload_prefix64) + payload_size + CODE_CAVE_PADDING * 2 + executable_header->p_filesz,
            map_size - executable_header->p_offset
        ),
        CODE_CAVE_PADDING
    );

    if (code_cave == NULL) {
        return (-1);
    }

    int offset = elf_map.header->e_entry - (code_cave - elf_map.start) - 7;

    memcpy(payload_prefix64 + 3, &offset, 4);
    memcpy(code_cave, payload_prefix64, sizeof(payload_prefix64));
    memcpy(code_cave + sizeof(payload_prefix64), payload, payload_size);

    executable_header->p_filesz = umax(
        executable_header->p_filesz,
        (code_cave - elf_map.start) + sizeof(payload_prefix64) + payload_size - executable_header->p_offset
    );
    executable_header->p_memsz = umax(
        executable_header->p_memsz,
        executable_header->p_filesz
    );
    elf_map.header->e_entry = code_cave - elf_map.start;
    return (0);
}