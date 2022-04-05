#include <stdio.h>
#include "common.h"
#include "fsutil.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct Song {
    int id;
    char title[50];
};

void print_song(struct Song *s) {
    printf("{\n");
    printf("\t%d\n", s->id);
    printf("\t%s\n", s->title);
    printf("}\n");
}

struct Song *song(char *title) {

}

int main() {
    drop_table("songs");
    create_table("songs", sizeof(struct Song));
    drop_test_index("songs");
    create_test_index("songs");
    struct LYHeader header;
    read_index_header("songs", &header);
    build_test_index("songs");
    return 0;
}
