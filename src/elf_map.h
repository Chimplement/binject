#ifndef ELF_MAP_H
# define ELF_MAP_H

# include <stddef.h>
# include <stdint.h>

# include "elf.h"

typedef struct
{
    union
    {
        uint8_t* start;
        Elf64_Ehdr* header;
    };
    Elf64_Phdr* program_headers;
    Elf64_Shdr* section_headers;
}   elf64_map_t;

elf64_map_t map_elf64(void* start, size_t size);

Elf64_Phdr* find_program_header(elf64_map_t elf_map, Elf64_Word type, Elf64_Word flags);

#endif