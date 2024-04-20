#include <string.h>

#include "elf_map.h"

bool is_elf(void* start, size_t size) {
    if (size < 4) {
        return (false);
    }
    return (!memcmp(start, "\x7f""ELF", 4));
}

uint8_t elf_class(void* start, size_t size) {
    if (size < 5) {
        return (0);
    }
    return (((uint8_t*)start)[EI_CLASS]);
}

elf32_map_t map_elf32(void* start, size_t size) {
    elf32_map_t elf_map;

    elf_map.start = start;

    if (size < 52) {
        return ((elf32_map_t){.start = NULL});
    }

    if (elf_map.header->e_phoff + elf_map.header->e_phnum * elf_map.header->e_phentsize > size) {
        return ((elf32_map_t){.start = NULL});
    }
    if (elf_map.header->e_shoff + elf_map.header->e_shnum * elf_map.header->e_shentsize > size) {
        return ((elf32_map_t){.start = NULL});
    }

    elf_map.program_headers = (Elf32_Phdr*)(elf_map.start + elf_map.header->e_phoff);
    elf_map.section_headers = (Elf32_Shdr*)(elf_map.start + elf_map.header->e_shoff);

    return (elf_map);
}

elf64_map_t map_elf64(void* start, size_t size) {
    elf64_map_t elf_map;

    elf_map.start = start;

    if (size < 64) {
        return ((elf64_map_t){.start = NULL});
    }

    if (elf_map.header->e_phoff + elf_map.header->e_phnum * elf_map.header->e_phentsize > size) {
        return ((elf64_map_t){.start = NULL});
    }
    if (elf_map.header->e_shoff + elf_map.header->e_shnum * elf_map.header->e_shentsize > size) {
        return ((elf64_map_t){.start = NULL});
    }

    elf_map.program_headers = (Elf64_Phdr*)(elf_map.start + elf_map.header->e_phoff);
    elf_map.section_headers = (Elf64_Shdr*)(elf_map.start + elf_map.header->e_shoff);

    return (elf_map);
}

Elf32_Phdr* find_program_header32(elf32_map_t elf_map, Elf32_Word type, Elf32_Word flags) {
    Elf32_Phdr* ph;

    for (unsigned int i = 0; i < elf_map.header->e_phnum; i++) {
        ph = elf_map.program_headers + i;
        if (ph->p_type == type && (ph->p_flags & flags) == flags) {
            return (ph);
        }
    }
    return (NULL);
}

Elf64_Phdr* find_program_header64(elf64_map_t elf_map, Elf64_Word type, Elf64_Word flags) {
    Elf64_Phdr* ph;

    for (unsigned int i = 0; i < elf_map.header->e_phnum; i++) {
        ph = elf_map.program_headers + i;
        if (ph->p_type == type && (ph->p_flags & flags) == flags) {
            return (ph);
        }
    }
    return (NULL);
}