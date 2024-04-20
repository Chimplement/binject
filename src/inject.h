#ifndef INJECT_H
# define INJECT_H

# include <stddef.h>

int inject32(void* map, size_t map_size, void* payload, size_t payload_size);
int inject64(void* map, size_t map_size, void* payload, size_t payload_size);

#endif