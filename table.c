#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/map.h"

bucket_t *bucket_init(const char ip[IP_LEN]) {
    if (ip == NULL) {
        perror("ip is null in bucket initialization");
        return NULL;
    }

    bucket_t *new_node = calloc(1, sizeof(bucket_t));

    if (new_node == NULL) {
        perror("Failed to initialize bucket");
        return NULL;
    }

    strncpy(new_node->ip, ip, IP_LEN - 1);

    new_node->requests = 0;

    return new_node;
}

table_t *table_init() {
    table_t *new_table = calloc(1, sizeof(table_t));

    if (new_table == NULL) {
        perror("Table failed to initialize");
        return NULL;
    }

    return new_table;
}

void table_print(const table_t *table) {
    if (table) {
        for (int index = 0; index < TABLE_LEN; index++) {
            bucket_t *bucket = table->buckets[index];
            while (bucket) {
                printf("%s - %d \n", bucket->ip, bucket->requests);
                bucket = bucket->next;
            }
        }
    } else {
        perror("Table is null");
    }
}

void table_free(table_t *table) {
    if (table) {
        for (int index = 0; index < TABLE_LEN; index++) {
            bucket_t *bucket = table->buckets[index];
            while (bucket) {
                bucket_t *temp = bucket->next;
                free(bucket);
                bucket = temp;
            }
            index++;
        }
    } else {
        perror("Table is null");
    }
}

int table_add(table_t *table, bucket_t *bucket) {
    if (table) {
        int index = hash_ip(bucket->ip);
        if (index < 0) {
            perror("IP is NULL");
            return -1;
        }

        if (table->buckets[index]) {
            bucket_t *temp = table->buckets[index];

            while (temp) {
                if (temp->ip == bucket->ip) {
                    temp->requests++;
                    return 0;
                }
                temp = temp->next;
            }
            bucket->next = temp;
        }
        table->buckets[index] = bucket;
        return 0;
    } else {
        perror("Table is NULL");
        return -1;
    }
}

bucket_t *table_get(table_t *table, const char ip[IP_LEN]) {
    if (table == NULL) {
        perror("Table is NULL");
        return NULL;
    } else if (ip == NULL) {
        perror("IP is NULL");
        return NULL;
    }

    int index = hash_ip(ip);
    bucket_t *ret = table->buckets[index];

    while (ret != NULL && strcmp(ret->ip, ip) != 0) {
        ret = ret->next;
    }

    if (ret) {
        return ret;
    } else {
        perror("IP not found in table");
        return NULL;
    }
}

int hash_ip(const char ip[IP_LEN]) {
    if (ip) {
        int index = 0;
        int num = 0;
        while (ip[index] != '\0') {
            num += ip[index];
            index++;
        }
        return num % TABLE_LEN;
    } else {
        perror("IP is NULL");
        return -1;
    }
}

int table_to_file(table_t *table, const char out_file[MAX_PATH]) {
    if (table == NULL) {
        perror("Table is NULL");
        return -1;
    } else if (out_file == NULL) {
        perror("outfile is NULL");
        return -1;
    }

    FILE *fd = fopen(out_file, "w");

    if (fd == NULL) {
        perror("File I/O failed");
        return -1;
    }

    for (int i = 0; i < TABLE_LEN; i++) {
        bucket_t *cur = table->buckets[i];

        while (cur) {
            fwrite(cur, sizeof(bucket_t), 1, fd);
            cur = cur->next;
        }
    }

    fclose(fd);
    return 0;
}

table_t *table_from_file(const char in_file[MAX_PATH]) {
    if (in_file == NULL) {
        perror("infile is NULL");
        return NULL;
    }

    table_t *table = table_init();

    if (table == NULL) {
        perror("Table is NULL");
        return NULL;
    }

    FILE *fd = fopen(in_file, "r");

    if (fd == NULL) {
        perror("Failed to read file");
        return NULL;
    }

    bucket_t temp;

    while (fread(&temp, sizeof(bucket_t), 1, fd) == 1) {
        bucket_t *added = bucket_init(temp.ip);

        if (added == NULL) {
            perror("Failed to initialize bucket from file");
            table_free(table);
            fclose(fd);
            return NULL;
        }

        added->requests = temp.requests;

        table_add(table, added);
    }

    fclose(fd);

    return table;
}
