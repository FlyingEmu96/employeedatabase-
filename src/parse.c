#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
    if (!dbhdr) return;
    printf("Employees (count=%u)\n", (unsigned)dbhdr->count);
    if (!employees || dbhdr->count == 0) {
        printf("(none)\n");
        return;
    }
    for (unsigned i = 0; i < dbhdr->count; i++) {
        printf("- %s | %s | hours=%u\n",
               employees[i].name,
               employees[i].address,
               (unsigned)employees[i].hours);
    }
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) return -1;
    if (!headerOut) return -1;
    struct dbheader_t *header = calloc(1, sizeof(*header));
    if (!header) return -1;
    ssize_t want = (ssize_t)sizeof(*header);
    ssize_t got  = read(fd, header, (size_t)want);
    if (got != want) { free(header); return -1; }
    header->version  = ntohs(header->version);
    header->count    = ntohs(header->count);
    header->magic    = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);
    if (header->magic != HEADER_MAGIC) { free(header); return -1; }
    if (header->version != 1) { free(header); return -1; }
    struct stat st;
    if (fstat(fd, &st) != 0) { free(header); return -1; }
    if ((unsigned int)st.st_size != header->filesize){ free(header); return -1; }
    *headerOut = header;
    return 0;
}

int create_db_header(struct dbheader_t **headerOut) {
    if (!headerOut) return -1;
    struct dbheader_t *header = calloc(1, sizeof(*header));
    if (!header) return -1;
    header->version  = 1;
    header->count    = 0;
    header->magic    = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);
    *headerOut = header;
    return 0;
}

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

int add_employee(struct dbheader_t *dbhdr,
                 struct employee_t **employeesOut,
                 char *addstring)
{
    if (!dbhdr || !employeesOut || !addstring) return -1;
    char *copy = strdup(addstring);
    if (!copy) return -1;
    char *name  = strtok(copy, ",");
    char *addr  = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");
    if (!name || !addr || !hours) { free(copy); return -1; }
    name  = trim(name);
    addr  = trim(addr);
    hours = trim(hours);
    char *endp = NULL;
    unsigned long h = strtoul(hours, &endp, 10);
    if (endp == hours || *endp != '\0') { free(copy); return -1; }
    struct employee_t emp;
    memset(&emp, 0, sizeof(emp));
    strncpy(emp.name, name, sizeof(emp.name)-1);
    strncpy(emp.address, addr, sizeof(emp.address)-1);
    emp.hours = (unsigned)h;
    unsigned old = dbhdr->count;
    struct employee_t *arr = *employeesOut;
    struct employee_t *newarr = realloc(arr, (size_t)(old + 1) * sizeof(*newarr));
    if (!newarr) { free(copy); return -1; }
    newarr[old] = emp;
    *employeesOut = newarr;
    dbhdr->count += 1;
    dbhdr->filesize += (unsigned int)sizeof(struct employee_t);
    free(copy);
    return 0;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut){
    if (fd < 0) return -1;
    if (!dbhdr || !employeesOut) return -1;
    unsigned count = dbhdr->count;
    if (count == 0) { *employeesOut = NULL; return 0; }
    struct employee_t *employees = calloc(count, sizeof(*employees));
    if (!employees) return -1;
    ssize_t need = (ssize_t)count * (ssize_t)sizeof(*employees);
    ssize_t got  = read(fd, employees, (size_t)need);
    if (got != need) { free(employees); return -1; }
    for (unsigned i = 0; i < count; i++) employees[i].hours = ntohl(employees[i].hours);
    *employeesOut = employees;
    return 0;
}

int output_file(int fd, struct dbheader_t *header, void *unused) {
    (void)unused;
    if (fd < 0) return -1;
    if (!header) return -1;
    header->magic    = htonl(header->magic);
    header->filesize = htonl(header->filesize);
    header->count    = htons(header->count);
    header->version  = htons(header->version);
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        header->magic    = ntohl(header->magic);
        header->filesize = ntohl(header->filesize);
        header->count    = ntohs(header->count);
        header->version  = ntohs(header->version);
        return -1;
    }
    ssize_t want = (ssize_t)sizeof(*header);
    ssize_t put  = write(fd, header, (size_t)want);
    if (put != want) {
        header->magic    = ntohl(header->magic);
        header->filesize = ntohl(header->filesize);
        header->count    = ntohs(header->count);
        header->version  = ntohs(header->version);
        return -1;
    }
    header->magic    = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);
    header->count    = ntohs(header->count);
    header->version  = ntohs(header->version);
    return 0;
}
