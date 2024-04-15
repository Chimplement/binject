#include "elf_map.h"

elf64_map_t map_elf64(void* start, size_t size) {
    elf64_map_t elf_map;

    elf_map.start = start;

    if (size < 64) {
        return (INVALID_ELF);
    }

    if (elf_map.header->e_phoff + elf_map.header->e_phnum * elf_map.header->e_phentsize > size) {
        return (INVALID_ELF);
    }
    if (elf_map.header->e_shoff + elf_map.header->e_shnum * elf_map.header->e_shentsize > size) {
        return (INVALID_ELF);
    }

    elf_map.program_headers = (Elf64_Phdr*)(elf_map.start + elf_map.header->e_phoff);
    elf_map.section_headers = (Elf64_Shdr*)(elf_map.start + elf_map.header->e_shoff);

    return (elf_map);
}

Elf64_Phdr* find_program_header(elf64_map_t elf_map, Elf64_Word type, Elf64_Word flags) {
    Elf64_Phdr* ph;

    for (unsigned int i = 0; i < elf_map.header->e_phnum; i++) {
        ph = elf_map.program_headers + i;
        if (ph->p_type == type && (ph->p_flags & flags) == flags) {
            return (ph);
        }
    }
    return (NULL);
}