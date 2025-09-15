/* 
    Data structure and function implementations for a 
    csv parsing module intended to convert from CSV files
    into C strings.
*/
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "record.h"

/* Returns a list of CSV records. */
struct csvRecord **readCSV(FILE *csvFile, int *n);

/* Read a line of input from the given file. */
char *getQuery(FILE *f);

/* Read the csv header row from `fp`,
 each dynamically allocated of exact string length. 
-> Return the Array of header strings*/
char **parse_header(FILE *input_file);

/* if any, strip trailing newline/CR (handles \n, \r, \r\n) */
void rstrip_newline(char **line);

/* Free a set of records. */
void freeCSV(struct csvRecord **dataset, int n);

/* Free the array of header strings*/
void freeHeader(char **headers, int n);


#endif 

