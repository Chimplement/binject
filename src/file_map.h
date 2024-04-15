#ifndef FILE_MAPPING_H
# define FILE_MAPPING_H

# include <unistd.h>

off_t get_file_size(int fd);

int map_file(int fd, int prot, int flags, void** map, size_t* map_size);

#endif