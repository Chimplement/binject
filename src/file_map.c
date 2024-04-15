#include <unistd.h>
#include <sys/mman.h>

#include "file_map.h"

off_t get_file_size(int fd) {
    off_t original_offset = lseek(fd, 0, SEEK_CUR);
    if (original_offset == (off_t)-1) {
        return (-1);
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    (void)lseek(fd, original_offset, SEEK_SET);
    if (file_size == (off_t)-1) {
        return (-1);
    }
    return (file_size);
}

int map_file(int fd, int prot, int flags, void** map, size_t* map_size) {
    off_t file_size = get_file_size(fd);
    if (file_size == (off_t)-1) {
        return (-1);
    }

    *map_size = file_size;
    *map = mmap(NULL, *map_size, prot, flags, fd, 0);
    if (*map == MAP_FAILED) {
        return (-1);
    }
    return (0);
}