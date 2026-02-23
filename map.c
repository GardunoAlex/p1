#include "./include/map.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./include/table.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: map <outfile> <infiles...>\n");
        return 1;
    }

    char *outfile = argv[1];
    table_t *table = table_init();

    for (int i = 2; i < argc; i++) {
        FILE *fd = fopen(argv[i], "r");

        if (fd) {
            char buffer[1024];
            log_line_t log;
            while (fgets(buffer, sizeof(buffer), fd)) {
                int filled = sscanf(buffer, "%19[^,],%15[^,],%7[^,],%36[^,],%3s", log.timestamp,
                                    log.ip, log.method, log.route, log.status);

                if (filled != 5) {
                    fprintf(stderr, "File %s failed to parse", argv[i]);
                    table_free(table);
                    fclose(fd);
                    return 1;
                }

                bucket_t *bucket = bucket_init(log.ip);

                if (bucket == NULL) {
                    perror("Failed to initialize bucket");
                    table_free(table);
                    fclose(fd);
                    return 1;
                }

                if (table_add(table, bucket) < 0) {
                    perror("Failed to add bucket to table");
                    table_free(table);
                    fclose(fd);
                    return 1;
                }
            }
        } else {
            perror("Failed to open file");
            table_free(table);
            return 1;
        }

        fclose(fd);
    }

    if (map_log(table, outfile) < 0) {
        perror("Failed to write table to file");
        table_free(table);
        return 1;
    }

    table_free(table);
    return 0;
}

int map_log(table_t *table, const char file_path[MAX_PATH]) {
    if (table_to_file(table, file_path) < 0) {
        fprintf(stderr, "Failed to write table to out file: %s\n", file_path);
        return -1;
    }

    return 0;
}
