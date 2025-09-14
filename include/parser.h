#ifndef PARSER_H
#define PARSER_H

#include "list.h"

#define MAX_LINE_LEN 511
#define MAX_FIELD_LEN 127

/* Read the csv header row from `fp` into `field_head` 
, each dynamically allocated of exact string length. */
int parse_header(FILE *fp, char **field_head);

/* parse all remaining lines into a list of record_t*.
 * returns a new list ( frees it with list_free(..., record_free)). */
list_t *parse_all_to_list(FILE *fp);

/* strip trailing newline/CR (handles \n, \r, \r\n) from a buffer read by fgets. */
void rstrip_newline(char *s);

/* free the header strings created by parse_header. */
void free_headers(char **field_head);

#endif 

