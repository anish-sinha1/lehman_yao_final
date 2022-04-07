#include <cslice.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "lehman-yao.h"

// function to resolve a table path based on its name
static const char *build_table_path(const char *table_name) {
    char *path = malloc(strlen(DATA_PATH) + strlen(table_name) + strlen(".bin"));
    strncpy(path, DATA_PATH, strlen(DATA_PATH));
    strncat(path, table_name, strlen(table_name));
    strncat(path, ".bin", strlen(".bin"));
    return path;
}

// function to resolve an index path based on its name
static const char *build_index_path(const char *table_name) {
    char *path = malloc(strlen(INDEX_PATH) + strlen(table_name) + strlen("_index.bin"));
    strncpy(path, INDEX_PATH, strlen(INDEX_PATH));
    strncat(path, table_name, strlen(table_name));
    strncat(path, "_index.bin", strlen("_index.bin"));
    return path;
}

// function to format a table based on its name. if it already exists it is dropped and reformatted
enum IO_STAT fmt_table(const char *table_name, size_t rec_size) {
    if (!table_name || rec_size < 0) return FMT_ERROR;
    const char *table_path = build_table_path(table_name);
    int fd = open(table_path, O_RDWR);
    if (fd < 0 && errno == ENOENT) fd = open(table_path, O_CREAT | O_EXCL | O_WRONLY);
    else truncate(table_path, 0);
    struct TableHeader table_header;
    strncpy(table_header.table_name, table_name, sizeof((struct TableHeader *) 0)->table_name);
    table_header.rec_size = rec_size;
    table_header.rec_ct = 0;
    table_header.current_max_id = 0;
    pwrite(fd, &table_header, sizeof(table_header), 0);
    close(fd);
    return FMT_SUCCESS;
}

// this function reads all the information from a table header into a buffer provided
static enum IO_STAT read_table_header(const char *table_name, struct TableHeader *header) {
    if (!table_name || !header) return READ_ERROR;
    const char *path = build_table_path(table_name);
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) return READ_ERROR;
    pread(fd, header, sizeof(*header), 0);
    close(fd);
    return READ_SUCCESS;
}

// this function writes a record into a table and updates the table's header
enum IO_STAT write_rec(const char *table_name, void *rec) {
    if (!table_name || !rec) return WRITE_ERROR;
    struct TableHeader header;
    if (read_table_header(table_name, &header) != READ_SUCCESS) return WRITE_ERROR;
    const char *path = build_table_path(table_name);
    int fd;
    if ((fd = open(path, O_RDWR)) < 0) return WRITE_ERROR;
    off_t offset = (off_t) (header.rec_ct * header.rec_size + sizeof(header));
    pwrite(fd, rec, header.rec_size, offset);
    close(fd);
    return WRITE_SUCCESS;
}

// this function overwrites a record with zeroes. the space is unfortunately still used but the record no longer exists
enum IO_STAT delete_rec(const char *table_name, LARGE_INT loc) {
    if (!table_name || loc < 0) return DELETE_ERROR;
    struct TableHeader header;
    if (read_table_header(table_name, &header) != READ_SUCCESS) return DELETE_ERROR;
    if (loc > header.rec_ct) return DELETE_ERROR;
    const char *path = build_table_path(table_name);
    int fd;
    if ((fd = open(path, O_RDWR)) < 0) return DELETE_ERROR;
    off_t offset = (off_t) (loc * header.rec_size * sizeof(header));
    char buf[header.rec_size];
    memset(buf, 0, sizeof(buf));
    pwrite(fd, &buf, sizeof(buf), offset);
    close(fd);
    return DELETE_SUCCESS;
}

// function to read a single record into memory
enum IO_STAT read_rec(const char *table_name, LARGE_INT loc, void *buf) {
    if (!table_name || !buf || loc < 0) return READ_ERROR;
    const char *path = build_table_path(table_name);
    struct TableHeader header;
    if (read_table_header(table_name, &header) != READ_SUCCESS) return READ_ERROR;
    if (loc > header.rec_ct) return READ_ERROR;
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) return READ_ERROR;
    off_t offset = ((off_t) header.rec_size * loc + (off_t) sizeof(header));
    pread(fd, buf, header.rec_size, offset);
    close(fd);
    return READ_SUCCESS;
}

// function to format an index
enum IO_STAT fmt_index(const char *table_name) {
    if (!table_name) return FMT_ERROR;
    struct TableHeader table_header;
    if (read_table_header(table_name, &table_header) != READ_SUCCESS) return FMT_ERROR;
    const char *index_path = build_index_path(table_name);
    int fd = open(index_path, O_RDWR);
    if (fd < 0 && errno == ENOENT) fd = open(index_path, O_CREAT | O_EXCL | O_WRONLY);
    struct IndexHeader index_header;
    strncpy(index_header.table_name, table_name, sizeof(((struct IndexHeader *) 0)->table_name));
    index_header.root_loc = -1;
    index_header.height = -1;
    index_header.node_ct = 0;
    index_header.order = TEST_INDEX_ORDER;
    pwrite(fd, &index_header, sizeof(index_header), 0);
    return FMT_SUCCESS;
}

// function to read the index's header into memory
enum IO_STAT read_index_header(const char *table_name, struct IndexHeader *index_header) {
    const char *index_path = build_index_path(table_name);
    int fd;
    if ((fd = open(index_path, O_RDONLY) < 0)) return READ_ERROR;
    pread(fd, index_header, sizeof(*index_header), 0);
    close(fd);
    return READ_SUCCESS;
}

// function to read the index's root into memory
enum IO_STAT read_index_root(const char *table_name, struct Node *node) {
    if (!table_name) return ROOT_ERROR;
    struct IndexHeader index_header;
    if (read_index_header(table_name, &index_header) != READ_SUCCESS) return ROOT_ERROR;
    if (index_header.root_loc == -1) return ROOT_NOT_EXISTS;
    const char *path = build_index_path(table_name);
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0) return ROOT_ERROR;
    pread(fd, node, sizeof(*node), 0);
    close(fd);
    return ROOT_SUCCESS;

}

// recursive function to insert a key into a non-full index page
static enum IO_STAT insert_nonfull_index_key(const char *table_name) {

}

// function to insert a key into the index
enum IO_STAT insert_index_key(const char *table_name) {
    struct Node root;
    struct IndexHeader index_header;
    if (read_index_header(table_name, &index_header) != READ_SUCCESS) return INSERT_ERROR;
    const char *index_path = build_index_path(table_name);
    int fd;
    if ((fd = open(index_path, O_RDWR)) < 0) return INSERT_ERROR;
    int root_stat = read_index_root(table_name, &root);
    if (root_stat == ROOT_ERROR) return INSERT_ERROR;
    if (root_stat == ROOT_NOT_EXISTS) {
        root.leaf = true;
        root.full = false;
        index_header.root_loc = 0;
        index_header.node_ct = 1;
        index_header.height = 0;
        pwrite(fd, &index_header, sizeof(index_header), 0);
        pwrite(fd, &root, sizeof(root), sizeof(index_header));
    }
    return INSERT_SUCCESS;
}

// function to actually build the index
enum IO_STAT build_index(const char *table_name) {
    struct TableHeader table_header;
    struct IndexHeader index_header;
    const char *table_path = build_table_path(table_name);
    if (read_table_header(table_name, &table_header) != READ_SUCCESS ||
        read_index_header(table_name, &index_header) != READ_SUCCESS)
        return INDEX_ERROR;
    LARGE_INT num_recs = table_header.rec_ct, num_pages = num_recs / index_header.order;
    size_t buf_size = index_header.order * table_header.rec_size;
    int fd;
    if ((fd = open(table_path, O_RDONLY)) < 0) return INDEX_ERROR;
    for (int i = 0; i < num_pages; i++) {
        char page[buf_size];
        off_t offset = (off_t) (sizeof(table_header) + (off_t) buf_size * i);
        pread(fd, &page, sizeof(page), offset);
    }
    return INDEX_SUCCESS;
}
