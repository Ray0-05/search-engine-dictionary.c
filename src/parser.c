#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "record.h"


/* split one csv record into FIELD_NUM tokens and store in `out_fields`.
 * Supports:
 *  - quoted fields "..." with doubled quotes "" inside,
 *  - commas inside quoted fields,
 *  - unquoted fields (no commas).
 * each token is heap-allocated (<= MAX_FIELD_LEN). returns number of tokens.
 * assumes lines are single-line (no embedded newlines to continue records). */
int split_csv_line(const char *line, char **out_fields) {
    // Initialise output array to NULL
    for (int i = 0; i < FIELD_NUM; i++) {
        out_fields[i] = NULL;
    }

    int field_count = 0;
    const char *p = line;

    while (*p && field_count < FIELD_NUM) {
        // Skip leading spaces/tabs before field
        while (*p == ' ' || *p == '\t') p++;

        char buf[MAX_FIELD_LEN + 1];
        int buf_index = 0;

        if (*p == '"') {
            // --- Quoted field ---
            p++; // Skip opening quote
            while (*p) {
                if (*p == '"' && p[1] == '"') {
                    // Escaped quote ("")
                    if (buf_index < MAX_FIELD_LEN) buf[buf_index++] = '"';
                    p += 2;
                } else if (*p == '"') {
                    // End of quoted field
                    p++;
                    while (*p == ' ' || *p == '\t') p++; // skip trailing spaces
                    break;
                } else {
                    if (buf_index < MAX_FIELD_LEN) buf[buf_index++] = *p;
                    p++;
                }
            }
        } else {
            // --- Unquoted field ---
            while (*p && *p != ',' && *p != '\n' && *p != '\r') {
                if (buf_index < MAX_FIELD_LEN) buf[buf_index++] = *p;
                p++;
            }
        }

        // Null-terminate buffer and save
        buf[buf_index] = '\0';
        out_fields[field_count] = strdup(buf);
        if (!out_fields[field_count]) {
            fprintf(stderr, "Error allocating memory for CSV field\n");
            exit(EXIT_FAILURE);
        }
        field_count++;

        // If current char is comma, move to next field
        if (*p == ',') {
            p++;
            // If this comma is right before end of string, add empty field
            if (*p == '\0' && field_count < FIELD_NUM) {
                out_fields[field_count] = strdup("");
                if (!out_fields[field_count]) {
                    fprintf(stderr, "Error allocating memory for empty CSV field\n");
                    exit(EXIT_FAILURE);
                }
                field_count++;
            }
        }
    }

    return field_count;
}


/* Read the csv header row from `fp` into `field_head` 
, each dynamically allocated of exact string length. 
-> Return the number of headers read in*/
int parse_header(FILE *fp, char **field_head) {
    char line[MAX_LINE_LEN + 2];  // room for newline + '\0'

    assert(fp != NULL);
    if (!fgets(line, sizeof (line), fp)) {
        // There's no header to be read in/there's an error
        return 0;
    }
    rstrip_newline(line);

    /* store each header into an index in `field_head` and 
        return number of fields read*/
    return split_csv_line(line, field_head);

}


/* parse all remaining lines into a list of address_t* */
list_t *parse_all_to_list(FILE *fp) {
    list_t *list = list_create();
    if (!list) return NULL;

    char line[MAX_LINE_LEN + 1];

    while (fgets(line, MAX_LINE_LEN, fp)) {
        rstrip_newline(line);

        char **fields = (char **)malloc(FIELD_NUM * sizeof(char *));
        assert(fields != NULL);
        int n = split_csv_line(line, fields);
        assert(n == FIELD_NUM);

        // create a record 
        address_t *rec = record_create(fields);

        list_append(list, rec);
    }

    return list;
}


/* if any, strip trailing newline/CR (handles \n, \r, \r\n) 
    from a buffer read by fgets */
void rstrip_newline(char *s) {
    int n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')){
        s[n - 1] = '\0';
    } 
}


/* free the header strings */
void free_headers(char **field_head) {
    for (int i = 0; i < FIELD_NUM; i++) free(field_head[i]);
    free(field_head);
}

