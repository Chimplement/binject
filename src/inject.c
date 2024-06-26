#include <elf.h>
#include <string.h>

#include "elf_map.h"
#include "mem_search.h"
#include "min_max.h"

#include "inject.h"

#define CODE_CAVE_PADDING 8

static uint8_t payload_prefix32[] = {
    0xe8, 0x00, 0x00, 0x00, 0x00,           // call 0x5
    0x58,                                   // pop  %eax
    0x2d, 0xff, 0xff, 0xff, 0xff,           // sub  $0xffffffff,%eax
    0x50,                                   // push %eax
};

int inject32(void* map, size_t map_size, void* payload, size_t payload_size) {
    elf32_map_t elf_map = map_elf32(map, map_size);
    if (elf_map.start == NULL) {
        return (-1);
    }

    Elf32_Phdr* executable_header = find_program_header32(elf_map, PT_LOAD, PF_R | PF_X);
    if (executable_header == NULL) {
        return (-1);
    }

    uint8_t* code_cave = (uint8_t*)find_code_cave(
        elf_map.start + executable_header->p_offset,
        sizeof(payload_prefix32) + payload_size,
        umin(
            sizeof(payload_prefix32) + payload_size + CODE_CAVE_PADDING * 2 + executable_header->p_filesz,
            map_size - executable_header->p_offset
        ),
        CODE_CAVE_PADDING
    );

    if (code_cave == NULL) {
        return (-1);
    }

    int offset = elf_map.header->e_entry - (code_cave - elf_map.start) - 5;
    offset = -offset;

    memcpy(payload_prefix32 + 7, &offset, 4);
    memcpy(code_cave, payload_prefix32, sizeof(payload_prefix32));
    memcpy(code_cave + sizeof(payload_prefix32), payload, payload_size);

    executable_header->p_filesz = umax(
        executable_header->p_filesz,
        (code_cave - elf_map.start) + sizeof(payload_prefix32) + payload_size - executable_header->p_offset
    );
    executable_header->p_memsz = umax(
        executable_header->p_memsz,
        executable_header->p_filesz
    );
    elf_map.header->e_entry = code_cave - elf_map.start;
    return (0);
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

    Elf64_Phdr* executable_header = find_program_header64(elf_map, PT_LOAD, PF_R | PF_X);
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