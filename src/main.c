#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>    // getopt portability
#include <getopt.h>
#include <stdlib.h>
#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(const char *prog) {
    printf("Usage: %s -n -f <database file>\n", prog);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database\n");
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile = false;
    int c;

    int dbfd = -1;
    struct dbhdr_t *dbhdr = NULL;

    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case '?':
                printf("Unknown option -%c\n", optopt ? optopt : '?');
                print_usage(argv[0]);
                return 2;
            default:
                return 2;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv[0]);
        return 2;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {              // or: if (dbfd < 0)
            printf("Unable to open database file\n");
            return -1;
        }
        if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            close(dbfd);
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {              // or: if (dbfd < 0)
            printf("Unable to open database file\n");
            return -1;
        }
        // TODO: read existing header here
    }

    printf("Newfile: %d\n", newfile ? 1 : 0);
    printf("filepath: %s\n", filepath);

    // cleanup (add before any early returns where needed)
    if (dbhdr) { free(dbhdr); dbhdr = NULL; }
    if (dbfd >= 0) close(dbfd);
    return 0;
}
