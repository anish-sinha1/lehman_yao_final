#ifndef LEHMAN_YAO_FINAL_FSUTIL_H
#define LEHMAN_YAO_FINAL_FSUTIL_H

#include "common.h"

struct TableHeader {
    char table_name[50];
    LARGE_INT rec_ct, current_max_id;
    size_t rec_size;
};

struct LYHeader {
    char table_name[50];
    int order;
    size_t page_size;
    LARGE_INT root_loc;
};

struct LYNode {
    LARGE_INT right_link;
    int irec_ct;
    char buf[8000 - sizeof(LARGE_INT) - sizeof(int)];
};

enum IO_STAT create_table(const char *table_name, size_t rec_size);
enum IO_STAT write_rec(const char *table_name, void *rec);
enum IO_STAT read_header(const char *table_name, struct TableHeader *header);
enum IO_STAT drop_table(const char *table_name);
enum IO_STAT read_rec(const char *table_name, LARGE_INT loc, void *rec);
enum IO_STAT create_test_index(const char *table_name);
enum IO_STAT drop_test_index(const char *table_name);
enum IO_STAT read_index_header(const char *table_name, struct LYHeader *header);
enum IO_STAT build_test_index(const char *table_name);

#endif