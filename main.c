#include <stdio.h>
#include "common.h"
#include "lehman-yao.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct Song {
    int id;
    char title[50];
};

int main() {
    fmt_table("songs", sizeof(struct Song));
    printf("%lu\n", sizeof(((struct Song *) 0)->title));
    return 0;
}
