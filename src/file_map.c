#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "file_map.h"

off_t get_file_size(int fd) {
    off_t original_offset = lseek(fd, 0, SEEK_CUR);
    if (original_offset == (off_t)-1) {
        return ((off_t)-1);
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    (void)lseek(fd, original_offset, SEEK_SET);
    if (file_size == (off_t)-1) {
        return ((off_t)-1);
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

void* load_file(char* path, size_t* file_size_buffer) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return (NULL);
    }

    off_t file_size = get_file_size(fd);
    if (file_size == (off_t)-1) {
        close(fd);
        return (NULL);
    }
    *file_size_buffer = file_size;

    void* buffer = malloc(file_size);
    if (buffer == NULL) {
        close(fd);
        return (NULL);
    }

    if (read(fd, buffer, file_size) == -1) {
        free(buffer);
        close(fd);
        return (NULL);
    }
    close(fd);
    return(buffer);
}