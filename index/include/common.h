#ifndef LEHMAN_YAO_FINAL_COMMON_H
#define LEHMAN_YAO_FINAL_COMMON_H

#include <string.h>

typedef long long int LARGE_INT;

#define DATA_PATH "/Users/anishsinha/Home/cprojects/lehman-yao-final/data/tables/"
#define INDEX_PATH "/Users/anishsinha/Home/cprojects/lehman-yao-final/data/index/"
#define PAGE_SIZE 8000
#define TEST_INDEX_ORDER 2;

enum IO_STAT {
    WRITE_SUCCESS, WRITE_ERROR,
    READ_SUCCESS, READ_ERROR,
    FMT_SUCCESS, FMT_ERROR,
    INDEX_SUCCESS, INDEX_ERROR,
    DROP_SUCCESS, DROP_ERROR,
    DELETE_SUCCESS, DELETE_ERROR,
    ROOT_NOT_EXISTS, ROOT_ERROR, ROOT_SUCCESS,
    INSERT_SUCCESS, INSERT_ERROR
};

#endif