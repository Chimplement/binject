#ifndef ELF_MAP_H
# define ELF_MAP_H

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

# include <elf.h>

typedef struct
{
    union
    {
        uint8_t* start;
        Elf32_Ehdr* header;
    };
    Elf32_Phdr* program_headers;
    Elf32_Shdr* section_headers;
}   elf32_map_t;

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

bool is_elf(void* start, size_t size);
uint8_t elf_class(void* start, size_t size);

elf32_map_t map_elf32(void* start, size_t size);
elf64_map_t map_elf64(void* start, size_t size);

Elf32_Phdr* find_program_header32(elf32_map_t elf_map, Elf32_Word type, Elf32_Word flags);
Elf64_Phdr* find_program_header64(elf64_map_t elf_map, Elf64_Word type, Elf64_Word flags);

#endif