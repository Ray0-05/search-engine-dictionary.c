#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "record.h"
#include "list.h"
#include "dict_linkedlist.h"
#include "io.h"
#include "parser.h"

#define EXPECTED_ARGC 4
#define STAGE_INDEX 1
#define INPUT_FILE_NAME_INDEX 2
#define OUTPUT_FILE_NAME_INDEX 3
#define EDZI_ADD 1
#define LINKED_LIST_STAGE "1"


int main (int argc, char *argv[]){
    
    FILE *input_file;
    FILE *output_file;
    
    // check if there's 4 arguments
    if (argc != 4){
        printf("Please enter exactly 4 arguments:\n\
                    1st: Call to program\n\
                    2nd: Stage Num\n\
                    3rd: input file name\n\
                    4th: output file name\n.");
        exit(EXIT_FAILURE);
    }

    input_file = fopen(argv[INPUT_FILE_NAME_INDEX], "r");
    output_file = fopen(argv[OUTPUT_FILE_NAME_INDEX], "w");
    assert(output_file != NULL);
    assert(input_file != NULL);

    /* Store headers for future use (printing output), 
    dynamically allocated for each field pointer and each field*/
    char **field_heads = (char **)malloc(FIELD_NUM * sizeof(char *));
    assert(field_heads != NULL);
    if(parse_header(input_file, field_heads) != FIELD_NUM){
        fprintf(stderr, "Check! Number of file headers read in is not as expected!");
        exit(EXIT_FAILURE);
    };

    /* Now finally excecute the program based on stage */
    if (strcmp(argv[STAGE_INDEX], LINKED_LIST_STAGE) == 0){                
        // Execute Stage 1

        // build link list
        list_t *list = parse_all_to_list(input_file);

        // build dictionary 
        dict_t *dict = build_dict_from_list(list, EDZI_ADD);

        // target that is to be search in dictionary
        char target[MAX_FIELD_LEN + 1];

        while (fgets(target, MAX_FIELD_LEN, stdin) != NULL) {
            // strip out '\n' and '\r' characters at the end
            rstrip_newline(target);
            
            /* search for the target in dictionary, 
            and print out results of search */
            search_stats_t *stats = search_data(dict, target, 
                                    output_file, field_heads);
            print_stats(stats, target);
            free(stats);
        }

        // free all the allocated 
        dict_list_free(dict); 
        free_headers(field_heads);
        dict = NULL; field_heads = NULL;
        

        fclose(input_file);
        fclose(output_file);
        
    }else{
        printf("Stage argument is invalid! Exiting program!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

