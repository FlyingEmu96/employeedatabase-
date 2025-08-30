#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include "parse.h"

extern int create_db_file(const char *path);
extern int  open_db_file(const char *path);

static void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file> [-a <employee_string>]\n", argv[0]);
    printf("\t-n              Create new database file\n");
    printf("\t-f <path>       (required) path to database file\n");
    printf("\t-a <string>     Add one employee: \"Name, Address, Hours\"\n");
}

int main(int argc, char *argv[]) {
    const char *addstring = NULL;
    const char *filepath  = NULL;
    bool newfile = false;
    int c;
    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:")) != -1){
        switch (c) {
            case 'n': newfile = true;         break;
            case 'f': filepath = optarg;      break;
            case 'a': addstring = optarg;     break;
            default:
                print_usage(argv);
                return -1;
        }
    }

    if (filepath == NULL){
        printf("Filepath is a required argument\n");
        print_usage(argv);
        return -1;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd < 0) return -1;
        if (create_db_header(&dbhdr) != 0) return -1;
        if (output_file(dbfd, dbhdr, NULL) != 0) return -1;
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd < 0) return -1;
        if (validate_db_header(dbfd, &dbhdr) != 0) return -1;
    }

    if (addstring){
        if (add_employee(dbhdr, &employees, (char*)addstring) != 0) return -1;
    }

    if (read_employees(dbfd, dbhdr, &employees) != 0) return -1;
    list_employees(dbhdr, employees);
    return 0;
}
