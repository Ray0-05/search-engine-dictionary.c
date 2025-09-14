/* this file focus on printing the data to output file and stats to stdout */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "record.h"
#include "io.h"


/* save the record of a node into output file */
void save_record(FILE *output_file, address_t *data, char *field_head[]) {
    
    fprintf(output_file, "-->");

    // print each field 
    for (int i = 0; i < FIELD_NUM - NON_STR_DATA; i++) {
         fprintf(output_file, " %s: %s ||", field_head[i], data->fields[i] ?  data->fields[i] : "");
    }

    // print x and y 
    for (int i = FIELD_NUM - NON_STR_DATA; i < FIELD_NUM; i++) {
        long double n = strtold(data->fields[i], NULL);
        fprintf(output_file, " %s: %.5Lf ||", field_head[i], n);
    }

    fprintf(output_file, "\n");
    fflush(output_file); // ensure data is written to file immediately 
}


/* print the tracked stats during the search into stdout */
void print_stats(search_stats_t *stats, char *target) {
    fprintf(stdout, "%s --> %d records found - comparisons: b%d n%d s%d\n", 
        target, stats->matches_count, stats->bit_count, stats->nodes_count, stats->string_count);
    return;
}


 

