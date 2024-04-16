#ifndef FILE_MAPPING_H
# define FILE_MAPPING_H

# include <unistd.h>

off_t get_file_size(int fd);

void* map_file(int fd, int prot, int flags, size_t* map_size);
void* map_file_at_path(const char* path, int prot, int flags, size_t* map_size);

void* alloc_file(int fd, size_t* file_size_buffer);
void* alloc_file_at_path(const char* path, size_t* file_size_buffer);

#endif