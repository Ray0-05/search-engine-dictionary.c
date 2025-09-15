/*
    Common dictionary functions:
    - Data record handling (struct data)
    - Query result handling (struct queryResult)
    - General printing functions

    These functions are shared between dictionary implementations 
    (e.g., linked list, Patricia tree).
*/

#include "dict_common.h"
#include "bit.h"
#include "record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NUM_FIELDS 35
#define NOTFOUND "NOTFOUND"

/* --------------------- Data Structures --------------------- */

/* A CSV record: all fields are strings */
struct data {
    char **fields;  // must contain exactly NUM_FIELDS entries
};

/* Query result returned from lookup */
struct queryResult {
    char *searchString;
    int numRecords;
    struct data **records;
    int bitCount;
    int nodeCount;
    int stringCount;
};

/* --------------------- Record Utilities --------------------- */

/* Build a data record from a csvRecord */
struct data *readRecord(struct csvRecord *record) {
    assert(record->fieldCount == NUM_FIELDS);
    struct data *ret = malloc(sizeof(struct data));
    assert(ret);

    ret->fields = malloc(sizeof(char *) * NUM_FIELDS);
    assert(ret->fields);

    for (int i = 0; i < NUM_FIELDS; i++) {
        ret->fields[i] = strdup(record->fields[i]);
        assert(ret->fields[i]);
    }
    return ret;
}

/* Free a single data record */
void freeData(struct data *d) {
    if (!d) return;
    for (int i = 0; i < NUM_FIELDS; i++) {
        if (d->fields[i]) free(d->fields[i]);
    }
    free(d->fields);
    free(d);
}

/* --------------------- Query Result Utilities --------------------- */

/* Free a query result */
void freeQueryResult(struct queryResult *r) {
    if (!r) return;
    free(r->records);
    free(r->searchString);
    free(r);
}

/* Print one field from a record */
void printField(FILE *f, struct data *record, int fieldIndex) {
    assert(record && record->fields && fieldIndex >= 0 && fieldIndex < NUM_FIELDS);
    fprintf(f, "%s", record->fields[fieldIndex]);
}

/* Print full query result (general, reused across dict types) */
void printQueryResult(struct queryResult *r, char **headers, FILE *summaryFile,
                      FILE *outputFile) {

    if (r->numRecords == 0) {
        fprintf(summaryFile,
                "%s --> %s - comparisons: b%d n%d s%d\n",
                r->searchString, NOTFOUND,
                r->bitCount, r->nodeCount, r->stringCount);
        fprintf(outputFile, "%s\n", r->searchString);
        fprintf(outputFile, "--> %s\n", NOTFOUND);
        return;
    } else {
        fprintf(summaryFile,
                "%s --> %d records found - comparisons: b%d n%d s%d\n",
                r->searchString, r->numRecords,
                r->bitCount, r->nodeCount, r->stringCount);
        }


    /* Print details to output file */
    fprintf(outputFile, "%s\n", r->searchString);
    for (int i = 0; i < r->numRecords; i++) {
        fprintf(outputFile, "--> ");
        for (int j = 0; j < NUM_FIELDS; j++) {
            fprintf(outputFile, "%s: ", headers[j]);
            printField(outputFile, r->records[i], j);
            fprintf(outputFile, " || ");
        }
        fprintf(outputFile, "\n");
    }
}
