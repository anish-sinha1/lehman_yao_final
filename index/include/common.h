#ifndef LEHMAN_YAO_FINAL_COMMON_H
#define LEHMAN_YAO_FINAL_COMMON_H

#include <string.h>

typedef long long int LARGE_INT;

#define DATA_BASE_PATH "/Users/anishsinha/Home/cprojects/lehman-yao-final/data/tables/"
#define DATA_BASE_PATH_LENGTH strlen(DATA_BASE_PATH)
#define INDEX_BASE_PATH "/Users/anishsinha/Home/cprojects/lehman-yao-final/data/index/"
#define INDEX_BASE_PATH_LENGTH strlen(INDEX_BASE_PATH)
#define PAGE_SIZE 8000

enum IO_STAT {
    WRITE_SUCCESS,
    WRITE_ERROR,
    READ_SUCCESS,
    READ_ERROR,
    FMT_SUCCESS,
    FMT_ERROR,
    INDEX_SUCCESS,
    INDEX_ERROR,
    DROP_SUCCESS,
    DROP_ERROR,
    DELETE_SUCCESS,
    DELETE_ERROR
};

#endif