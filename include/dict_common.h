#ifndef DICT_COMMON_H
#define DICT_COMMON_H

#include <stdio.h>
#include "record.h"

/* --------------------- Constants --------------------- */

#define NUM_FIELDS 35

/* --------------------- Forward Declarations --------------------- */

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

/* --------------------- Function Prototypes --------------------- */

/* Build a data record from a csvRecord (deep copy of fields) */
struct data *readRecord(struct csvRecord *record);

/* Free a data record */
void freeData(struct data *d);

/* Free a query result */
void freeQueryResult(struct queryResult *r);

/* Print one field from a record */
void printField(FILE *f, struct data *record, int fieldIndex);

/* Print a query result (summary + details) */
void printQueryResult(struct queryResult *r, char ** headers, FILE *summaryFile,
                      FILE *outputFile);

#endif
