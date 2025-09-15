/* 
    Data structure and function implementations for a 
    csv parsing module intended to convert from CSV files
    into C strings.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "read.h"
#include "record.h"
#include "record.c"

#define INIT_RECORDS 1
#define NUM_FIELDS 35
#define MAX_RECORD_LEN 512  // 511 chars + '\0'

/* 
Reads a line - removing trailing whitespace, returns a csvRecord or NULL
if parsing was unsuccessful. 
*/
struct csvRecord *parseLine(char *line);
/*
Uses the parsing structure to see if the next line from the CSV needs to
be included.
*/
void checkLine(char **line, FILE *csvFile, char **line2);
/* Used to clean the tracing newline / carriage*/
void rstrip_newline(char **line);

struct csvRecord **readCSV(FILE *csvFile, int *num_records){
    struct csvRecord **records = NULL;
    int numRecords = 0;
    int spaceRecords = 0;

    char *line = malloc(MAX_RECORD_LEN);
    assert(line);

    /* Used to speed up checkLine function. */
    char *line2 = malloc(MAX_RECORD_LEN);
    assert(line2);

    while(fgets(line, MAX_RECORD_LEN, csvFile) != NULL){
        if(spaceRecords == 0){
            records = (struct csvRecord **) 
                malloc(sizeof(struct csvRecord *) * INIT_RECORDS);
            assert(records);
            spaceRecords = INIT_RECORDS;
        } else if(numRecords >= spaceRecords){
            spaceRecords *= 2;
            records = (struct csvRecord **)
                realloc(records, sizeof(struct csvRecord *) * spaceRecords);
            assert(records);
        }
        /* If the line ends in an open double quote, we may need to extend the 
            line. */
        checkLine(&line, csvFile, &line2);
        records[numRecords] = parseLine(line);
        if(records[numRecords]){
            numRecords++;
        }
    }
    if(line){
        free(line);
    }
    if(line2){
        free(line2);
    }

    /* Shrink. */
    records = (struct csvRecord **)
                realloc(records, sizeof(struct csvRecord *) * numRecords);
    assert(records);

    *num_records = numRecords;
    return records;
}

void checkLine(char **line, FILE *csvFile, char **line2) {
    // char *line2 = NULL;
    // size_t size2 = 0;
    int lineLen = strlen(*line);
    int progress = 0;
    int inQuotes = 0;
    while(progress <= lineLen) {
        if((*line)[progress] == '\"'){
            /* Flip inQuotes value. */
            if(inQuotes){
                inQuotes = 0;
            } else {
                inQuotes = 1;
            }
        } else if ((*line)[progress] == '\0'){
            /* If in quotes, include following line, otherwise, end of 
                string. */
            if(inQuotes){
                /* CSV is malformed if there is not an end quote. */
                assert(fgets(*line2, MAX_RECORD_LEN, csvFile) != NULL);
                size_t line2Len = strlen(*line2);
                if((lineLen + line2Len + 1) > MAX_RECORD_LEN){
                    fprintf(stderr, "A record has exceeded the maximum length(%d) it can have, please check!", \
                        MAX_RECORD_LEN);
                    free(line);
                    free(line2);
                    exit(EXIT_FAILURE);
                }
                strcat(*line, *line2);
                lineLen = strlen(*line);
                /* \0 is deleted from *line, so we need to decrement progress by one. */
                progress--;
            }
        }
        progress++;
    }
}

/* This function reads CSVs. */
struct csvRecord *parseLine(char *line){
    struct csvRecord *ret = NULL;
    char **fields = (char **) malloc(sizeof(char *) * NUM_FIELDS);
    assert(fields);
    int fieldNum = 0;
    int len = strlen(line);
    
    /* Remove trailing whitespace first. */
    while(len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')){
        line[len - 1] = '\0';
        len--;
    }
    /* Check for empty lines. */
    if(len == 0){
        return NULL;
    }

    int progress = 0;
    int start = 0;
    /* For simplicity assume quotes only escape comma fields. */
    int inQuotes = 0;

    while(progress <= len){
        if(line[progress] == '\"'){
            if(inQuotes){
                inQuotes = 0;
            } else {
                inQuotes = 1;
            }
        } else if(line[progress] == ',' || line[progress] == '\0'){
            if(inQuotes){
                /* Skip over, comma is escaped. */
                /* Verify CSV not malformed. */
                assert(line[progress] != '\0');
            } else {
                /* Check fields match expected. */
                assert(line[progress] != '\0' || fieldNum == (NUM_FIELDS - 1));
                /* Terminate */
                line[progress] = '\0';
                assert(fieldNum < NUM_FIELDS);
                fields[fieldNum] = strdup(line + start);
                assert(fields[fieldNum]);
                fieldNum++;
                start = progress + 1;
            }
        }
        progress++;
    }
    /* Sanity check! Did we get everything? */
    assert(fieldNum == NUM_FIELDS);

    /* Step 2: Clean extraneous quotes. */
    for(int i = 0; i < NUM_FIELDS; i++){
        if(strlen(fields[i]) > 0){
            if(fields[i][0] == '\"'){
                assert(fields[i][strlen(fields[i]) - 1] == '\"');
                fields[i][strlen(fields[i]) - 1] = '\0';
                int len = strlen(fields[i]);
                for(int j = 0; j < len; j++){
                    // Shuffle all characters along one character.
                    fields[i][j] = fields[i][j + 1];
                }
            }
        }
    }

    /* Step 3: Reduce quote count where occuring. */
    for(int i = 0; i < NUM_FIELDS; i++){
        if(strlen(fields[i]) > 0){
            /* Progress pointer - marks how much of the string has 
                been written. */
            int progress = 0;
            for(int j = 0; j <= strlen(fields[i]); j++){
                if(fields[i][j] == '\"'){
                    /* Quotes always appear in pairs, so skip over first 
                        quote. */
                    j++;
                }
                if(j != progress){
                    fields[i][progress] = fields[i][j];
                }
                progress++;
            }
        }
    }

    ret = (struct csvRecord *) malloc(sizeof(struct csvRecord));
    assert(ret);
    ret->fieldCount = fieldNum;
    ret->fields = fields;

    return ret;
}

char *getQuery(FILE *f){
    char *line = malloc(MAX_RECORD_LEN);
    assert(line);
    if(fgets(line, MAX_RECORD_LEN, f) != NULL){
        rstrip_newline(&line);
        return line;
    } else {
        if(line){
            free(line);
        }
        return NULL;
    }
}

/* Read the csv header row from `fp`,
 each dynamically allocated of exact string length. 
-> Return the Array of header strings*/
char **parse_header(FILE *input_file) {
    char *line = malloc(MAX_RECORD_LEN);
    assert(line);
    char **headers = malloc(sizeof(*headers) * NUM_FIELDS);

    assert(input_file != NULL);

    if (fgets(line, MAX_RECORD_LEN, input_file) == NULL) {
        free(headers);
        return NULL; // No header / error
    }

    // Trim trailing newline or carriage return
    rstrip_newline(&line);

    char *token = strtok(line, ",");
    int count = 0;

    while (token != NULL && count < NUM_FIELDS) {
        headers[count] = strdup(token); 
        count++;
        token = strtok(NULL, ",");
    }

    free(line); // free buffer allocated by malloc

    return headers;
}

/* if any, strip trailing newline/CR (handles \n, \r, \r\n) */
void rstrip_newline(char **line) {
    if (!line || !*line) return;
    size_t L = strlen(*line);
    while (L > 0 && (((*line)[L-1] == '\n') || ((*line)[L-1] == '\r'))) {
        (*line)[--L] = '\0';
    }
}


void freeCSV(struct csvRecord **dataset, int num_records){
    if(! dataset){
        return;
    }
    for(int i = 0; i < num_records; i++){
        for(int j = 0; j < dataset[i]->fieldCount; j++){
            free(dataset[i]->fields[j]);
        }
        free(dataset[i]->fields);
        free(dataset[i]);
    }
    free(dataset);
}

void freeHeader(char **headers, int n){
    for (int i = 0; i < n; i++){
        free(headers[i]);
    }
    free(headers);
}
