#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "fsutil.h"

enum IO_STAT create_table(const char *table_name, size_t rec_size) {
    if (!table_name) return FMT_ERROR;
    struct TableHeader header;
    strncpy(header.table_name, table_name, 50);
    header.rec_ct = 0;
    header.rec_size = rec_size;
    header.current_max_id = 0;
    char path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(path, DATA_BASE_PATH, DATA_BASE_PATH_LENGTH);
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    int fd = open(path, O_RDWR);
    if (fd < 0 && ENOENT == errno) {
        fd = open(path, O_CREAT | O_EXCL | O_WRONLY);
    }
    truncate(path, 0);
    pwrite(fd, &header, sizeof(header), 0);
    close(fd);
    return FMT_SUCCESS;
}

enum IO_STAT read_header(const char *table_name, struct TableHeader *header) {
    char path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(path, DATA_BASE_PATH, sizeof(path));
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) return READ_ERROR;
    pread(fd, header, sizeof(struct TableHeader), 0);
    close(fd);
    return READ_SUCCESS;
}

enum IO_STAT write_rec(const char *table_name, void *rec) {
    char path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(path, DATA_BASE_PATH, DATA_BASE_PATH_LENGTH);
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    struct TableHeader header;
    if (read_header("songs", &header) != READ_SUCCESS) return WRITE_ERROR;
    int fd;
    if ((fd = open(path, O_WRONLY)) < 0) return WRITE_ERROR;
    off_t offset = (off_t) (sizeof(struct TableHeader) + header.rec_ct * header.rec_size);
    header.rec_ct++;
    header.current_max_id++;
    pwrite(fd, &header, sizeof(struct TableHeader), 0);
    pwrite(fd, rec, header.rec_size, offset);
    close(fd);
    return WRITE_SUCCESS;
}

enum IO_STAT drop_table(const char *table_name) {
    if (!table_name) return DROP_ERROR;
    char path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(path, DATA_BASE_PATH, DATA_BASE_PATH_LENGTH);
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    truncate(path, 0);
    return DROP_SUCCESS;
}

enum IO_STAT read_rec(const char *table_name, LARGE_INT loc, void *rec) {
    if (!table_name || !rec || loc < 0) return READ_ERROR;
    char path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(path, DATA_BASE_PATH, sizeof(path));
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) return READ_ERROR;
    struct TableHeader header;
    if (read_header(table_name, &header) != READ_SUCCESS) return READ_ERROR;
    if (header.rec_ct < loc) return READ_ERROR;
    off_t offset = (off_t) (sizeof(struct TableHeader) + loc * header.rec_size);
    pread(fd, rec, header.rec_size, offset);
    close(fd);
    return READ_SUCCESS;
}


enum IO_STAT drop_test_index(const char *table_name) {
    if (!table_name) return DROP_ERROR;
    char index_path[INDEX_BASE_PATH_LENGTH + strlen(table_name) + strlen("_index.bin")];
    strncpy(index_path, INDEX_BASE_PATH, sizeof(index_path));
    strncat(index_path, table_name, strlen(table_name));
    strncat(index_path, "_index.bin", strlen("_index.bin"));
    int fd;
    if ((fd = open(index_path, O_RDWR)) < 0) return DROP_ERROR;
    close(fd);
    truncate(index_path, 0);
    return DROP_SUCCESS;
}

enum IO_STAT read_index_header(const char *table_name, struct LYHeader *header) {
    char index_path[INDEX_BASE_PATH_LENGTH + strlen(table_name) + strlen("_index.bin")];
    strncpy(index_path, INDEX_BASE_PATH, sizeof(index_path));
    strncat(index_path, table_name, strlen(table_name));
    strncat(index_path, "_index.bin", strlen("_index.bin"));
    int fd;
    if ((fd = open(index_path, O_RDONLY)) < 0) return READ_ERROR;
    pread(fd, header, sizeof(struct LYHeader), 0);
    close(fd);
    return READ_SUCCESS;
}

enum IO_STAT create_test_index(const char *table_name) {
    char table_path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(table_path, DATA_BASE_PATH, sizeof(table_path));
    strncat(table_path, table_name, strlen(table_name));
    strncat(table_path, ".bin", strlen(".bin"));
    struct TableHeader header;
    if (read_header("songs", &header) != READ_SUCCESS) return FMT_ERROR;
    // in this simple test case there is no distinction between the size of an internal node versus a leaf node
    // internal nodes contain id & next node location and leaf nodes contain id & table position location. both cases
    // use 16 bytes. also, the order is set as 2 so there is very easy testing and a lot of data doesn't need to be
    // procured
    struct LYHeader index_header;
    strncpy(index_header.table_name, table_name, 50);
    index_header.order = 2;
    index_header.page_size = PAGE_SIZE;
    index_header.root_loc = 0;
    char index_path[INDEX_BASE_PATH_LENGTH + strlen(table_name) + strlen("_index.bin")];
    strncpy(index_path, INDEX_BASE_PATH, sizeof(index_path));
    strncat(index_path, table_name, strlen(table_name));
    strncat(index_path, "_index.bin", strlen("_index.bin"));
    int fd = open(index_path, O_RDWR);
    if (fd < 0 && errno == ENOENT) fd = open(index_path, O_CREAT | O_EXCL | O_WRONLY);
    truncate(index_path, 0);
    pwrite(fd, &index_header, sizeof(struct LYHeader), 0);
    close(fd);
    return FMT_SUCCESS;
}

enum IO_STAT build_test_index(const char *table_name) {
    char index_path[INDEX_BASE_PATH_LENGTH + strlen(table_name) + strlen("_index.bin")];
    strncpy(index_path, INDEX_BASE_PATH, sizeof(index_path));
    strncat(index_path, table_name, strlen(table_name));
    strncat(index_path, "_index.bin", strlen("_index.bin"));
    struct LYHeader index_header;
    if (read_index_header("songs", &index_header) != READ_SUCCESS) return INDEX_ERROR;
    char table_path[DATA_BASE_PATH_LENGTH + strlen(table_name) + strlen(".bin")];
    strncpy(table_path, DATA_BASE_PATH, sizeof(table_path));
    strncat(table_path, table_name, strlen(table_name));
    strncat(table_path, ".bin", strlen(".bin"));
    struct TableHeader table_header;
    if (read_header(index_header.table_name, &table_header) != READ_SUCCESS) return INDEX_ERROR;
    char buf[PAGE_SIZE];
    printf("current max id: %lld\n", table_header.current_max_id);
}