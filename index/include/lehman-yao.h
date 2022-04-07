#ifndef LEHMAN_YAO_FINAL_LEHMAN_YAO_H
#define LEHMAN_YAO_FINAL_LEHMAN_YAO_H

#include <stdbool.h>
#include <stdlib.h>

#include "common.h"

struct TableHeader {
    char table_name[50];
    LARGE_INT rec_ct, current_max_id;
    size_t rec_size;
};

struct IndexHeader {
    char table_name[50];
    LARGE_INT root_loc, node_ct;
    int order, height;
};

struct Node {
    bool full, leaf;
    char data[PAGE_SIZE - 2 * sizeof(bool)];
};

enum IO_STAT fmt_table(const char *table_name, size_t rec_size);

#endif