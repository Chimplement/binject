#include "min_max.h"

#include "mem_search.h"

size_t mem_count(void* start, char byte, size_t max_repeat) {
    size_t repeat = 0;
    while (repeat < max_repeat && ((char*)start)[repeat] == byte) {
        repeat++;
    }
    return (repeat);
}

void* find_code_cave(void* start, size_t min_cave_size, size_t search_length, size_t padding) {
    size_t offset = 0;
    min_cave_size += padding * 2;
    while (search_length - offset >= min_cave_size) {
        size_t zero_count = mem_count((char*)start + offset, 0, min_cave_size);
        if (zero_count >= min_cave_size) {
            return ((char*)start + offset + padding);
        }
        offset += umax(zero_count, 1);
    }
    return (NULL);
}