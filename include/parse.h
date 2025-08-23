#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations to avoid dragging heavy headers into every user. */
struct dbheader_t;
struct employee_t;

/* Status fallbacks (in case common.h isn't included before this file). */
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR   -1
#endif

/* Create a new database header (tests call: create_db_header(&dbhdr)) */
int create_db_header(struct dbheader_t **headerOut);

/* Validate a database header. Should return STATUS_SUCCESS/STATUS_ERROR. */
int validate_db_header(struct dbheader_t *dbhdr);

/* List employees (stub is fine for now). */
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);

/* Add a single employee parsed from addstring.
   Tests call: add_employee(dbhdr, &employees, (char*)test_employee) */
int add_employee(struct dbheader_t *dbhdr,
                 struct employee_t **employees,
                 char *addstring);

/* Read employees from fd into a dynamically allocated array. */
int read_employees(int fd,
                   struct dbheader_t *dbhdr,
                   struct employee_t **employeesOut);

/* Write the database (header + employees) to fd. */
int output_file(int fd,
                struct dbheader_t *dbhdr,
                struct employee_t *employees);

#ifdef __cplusplus
}
#endif

#endif /* PARSE_H */
