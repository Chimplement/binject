#include <stdint.h>
#include <elf.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <sys/mman.h>

#include "min_max.h"
#include "mem_search.h"
#include "str_manip.h"
#include "file_io.h"
#include "elf_map.h"
#include "inject.h"
#include "help.h"

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) {
        help(argv[0]);
        return (0);
    }

    size_t map_size;
    uint8_t* map = map_file_at_path(argv[1], PROT_READ | PROT_WRITE, MAP_PRIVATE, &map_size);
    if (map == MAP_FAILED) {
        exit_error("failed to load target file");
    }

    size_t payload_size;
    uint8_t* payload = alloc_file_at_path(argv[2], &payload_size);
    if (payload == NULL) {
        (void)munmap(map, map_size);
        exit_error("failed to load payload");
    }

    if (!is_elf(map, map_size)) {
        (void)munmap(map, map_size);
        free(payload);
        exit_error("target file doesn't use the ELF format");
    }

    switch (elf_class(map, map_size))
    {
        case ELFCLASS32:
            if (inject32(map, map_size, payload, payload_size) == -1) {
                (void)munmap(map, map_size);
                free(payload);
                exit_error("failed to find a suitable location for the payload");
            }
            break;
        case ELFCLASS64:
            if (inject64(map, map_size, payload, payload_size) == -1) {
                (void)munmap(map, map_size);
                free(payload);
                exit_error("failed to find a suitable location for the payload");
            }
            break;
        default:
            (void)munmap(map, map_size);
            free(payload);
            exit_error("target file doesn't use the ELF format");
    }

    free(payload);

    char* out_file_name = str_join(basename(argv[1]), ".infected");
    if (out_file_name == NULL) {
        (void)munmap(map, map_size);
        exit_error("failed to create the output file");
    }

    printf("Created: %s\n", out_file_name);
    int out_fd = open(out_file_name, O_CREAT | O_WRONLY, 0744);
    free(out_file_name);
    if (out_fd == -1){
        (void)munmap(map, map_size);
        exit_error("failed to create the output file");
    }

    ssize_t bytes_written = write(out_fd, map, map_size);
    close(out_fd);
    (void)munmap(map, map_size);
    if (bytes_written == -1) {
        exit_error("failed to write to the output file");
    }
    
    return (0);
}