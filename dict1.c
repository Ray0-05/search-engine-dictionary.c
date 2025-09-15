#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "record.h"
#include "dict_common.h"
#include "linked_list_dict.h"
#include "read.h"

#define EXPECTED_ARGC 4
#define STAGE_INDEX 1
#define INPUT_FILE_NAME_INDEX 2
#define OUTPUT_FILE_NAME_INDEX 3
#define EDZI_ADD 1
#define LINKED_LIST_STAGE "1"
#define PATRICIA_TREE_STAGE "2"


int main (int argc, char *argv[]){
    // check if there's 4 arguments and the stage input is correct
    if (argc != 4){
        printf("Please enter exactly 4 arguments:\n\
                    1st: Call to program\n\
                    2nd: Stage Num\n\
                    3rd: input file name\n\
                    4th: output file name\n.");
        exit(EXIT_FAILURE);
    } 
    if (strcmp(argv[STAGE_INDEX], PATRICIA_TREE_STAGE) == 0) {
        printf("To run the Patricia tree dictionary, please compile and run the dict2.c file!");
        exit(EXIT_FAILURE);
    } else if (strcmp(argv[STAGE_INDEX], LINKED_LIST_STAGE) != 0) {
        printf("This program is only for running the Linked List dictionary,\n"
            "for more information on how to run the proper program, please read the README file\n");
        exit(EXIT_FAILURE);
    }


                                            /* Now Execute Stage 1*/

    FILE *input_file = fopen(argv[INPUT_FILE_NAME_INDEX], "r");
    FILE *output_file = fopen(argv[OUTPUT_FILE_NAME_INDEX], "w");
    assert(output_file != NULL);
    assert(input_file != NULL);

    /* Store headers for future use (printing output), freed below*/
    char **field_headers = parse_header(input_file);
    assert(field_headers != NULL);

    int num_records;
    struct csvRecord **dataset = readCSV(input_file, &num_records);

    // Build linked list dictionary with the Edzi_add field as search key
    struct llDict *dict = llDictNew(EDZI_ADD);

    // pPopulate dictionary
    for (int i = 0; i < num_records; i++) {
        llDictInsert(dict, dataset[i]);
    }

    char *query = NULL;
    // search for queries
    while ((query = getQuery(stdin)) != NULL) {        
        /* search for the target in dictionary, 
        and print out results of search */
        struct queryResult *results = llDictLookup(dict, query);
        printQueryResult(results, field_headers, stdout, output_file);
        freeQueryResult(results);
    }

    // free all the allocated 
    llDictFree(dict); 
    freeHeader(field_headers, NUM_FIELDS);
    freeCSV(dataset, num_records);
    dict = NULL; 
    field_headers = NULL; 
    dataset = NULL;
    

    fclose(input_file);
    fclose(output_file);

    return 0;
}

