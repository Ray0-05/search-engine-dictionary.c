#include <stdlib.h>
#include <stdio.h>
#include "record.h"

/* record_create: Build an address_t record from parsed CSV fields.
 *
 * Parameters:
 *    char *fields[FIELD_NUM] – array of strings from the parser, each
 *                       representing one CSV column (heap-allocated).
 *
 * Returns:
 *   A pointer to a newly created record (address_t).
 *
 * Notes:
 *   - This function is the abstraction boundary: if address_t is later
 *     redefined (e.g., changed from an array-of-strings to a struct),
 *     only this module and the dictionary module need to
 *     change. Parser code stays the same as well as the driver module.
 */
address_t *record_create(char **fields) {
    address_t *rec = malloc(sizeof(address_t));
    if (!rec) {
        fprintf(stderr, "Error allocating memory for record\n");
        exit(EXIT_FAILURE);
    }

    rec->fields = fields;  // just take ownership of the array
    return rec;
}


/* This function frees memory allocated to store an adress record*/
void record_free(address_t *rec) {
    if (!rec) return;

    if (rec->fields) {
        for (int i = 0; i < FIELD_NUM; i++) {
            free(rec->fields[i]);  // free each strdup’d string
        }
        free(rec->fields);  // free the array of char*
    }

    free(rec);  // free the struct
}

