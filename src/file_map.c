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

void* map_file(int fd, int prot, int flags, size_t* map_size) {
    off_t file_size = get_file_size(fd);
    if (file_size == (off_t)-1) {
        return (MAP_FAILED);
    }

    *map_size = file_size;
    void* map = mmap(NULL, *map_size, prot, flags, fd, 0);
    if (map == MAP_FAILED) {
        return (MAP_FAILED);
    }
    return (map);
}

void* map_file_at_path(const char* path, int prot, int flags, size_t* map_size) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return (MAP_FAILED);
    }
    void* map = map_file(fd, prot, flags, map_size);
    (void)close(fd);
    return(map);
}

void* alloc_file(int fd, size_t* file_size_buffer) {
    off_t file_size = get_file_size(fd);
    if (file_size == (off_t)-1) {
        return (NULL);
    }
    *file_size_buffer = file_size;

    void* buffer = malloc(file_size);
    if (buffer == NULL) {
        return (NULL);
    }

    if (read(fd, buffer, file_size) == -1) {
        free(buffer);
        return (NULL);
    }
    return(buffer);
}

void* alloc_file_at_path(const char* path, size_t* file_size_buffer) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return (NULL);
    }
    void* buffer = alloc_file(fd, file_size_buffer);
    (void)close(fd);
    return(buffer);
}