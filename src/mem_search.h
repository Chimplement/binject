#ifndef MEM_SEARCH_H
# define MEM_SEARCH_H

# include <stddef.h>

size_t mem_count(void* start, char byte, size_t max_repeat);

void* find_code_cave(void* start, size_t min_cave_size, size_t search_length, size_t padding);

#endif