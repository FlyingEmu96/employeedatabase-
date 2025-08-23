#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations to avoid pulling all headers here */
struct dbheader_t;
struct employee_t;

/* Fallback status codes (in case common.h doesn't define them) */
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR   -1
#endif

/* Create a new, zero-initialized database header.
 * NOTE: fd parameter removed to match the tests: create_db_header(&dbhdr)
 */
int create_db_header(struct dbheader_t **headerOut);

/* Validate a database header. Returns STATUS_SUCCESS/STATUS_ERROR. */
int validate_db_header(struct dbheader_t *dbhdr);

/* List employees (no-op stub for now). */
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);

/* Add a single employee parsed from addstring.
 * The tests call: add_employee(dbhdr, &employees, (char*)test_employee)
 * so employees is a ** (may be reallocated).
 */
int add_employee(struct dbheader_t *dbhdr,
                 struct employee_t **employees,
                 char *addstring);

/* Read employees from an fd into a dynamically allocated array. */
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
