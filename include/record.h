
/* This file defines the shared transfer data structure csvRecord
    used to hold a set of read fields. */
    
#ifndef RECORD_H
#define RECORD_H

#define NUM_FIELDS 35

struct csvRecord {
    int fieldCount;
    char **fields;
};
#endif


