#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include "list.h"
#include "dict_linkedlist.h"

/* Field indices for CSV columns so field type is not lost*/
#define PFI         0
#define EZI_ADD     1
#define SRC_VERIF   2
#define PROPSTATUS  3
#define GCODEFEAT   4
#define LOC_DESC    5
#define BLGUNTTYP   6
#define HSAUNITID   7
#define BUNIT_PRE1  8
#define BUNIT_ID1   9
#define BUNIT_SUF1  10
#define BUNIT_PRE2  11
#define BUNIT_ID2   12
#define BUNIT_SUF2  13
#define FLOOR_TYPE  14
#define FLOOR_NO_1  15
#define FLOOR_NO_2  16
#define BUILDING    17
#define COMPLEX     18
#define HSE_PREF1   19
#define HSE_NUM1    20
#define HSE_SUF1    21
#define HSE_PREF2   22
#define HSE_NUM2    23
#define HSE_SUF2    24
#define DISP_NUM1   25
#define ROAD_NAME   26
#define ROAD_TYPE   27
#define RD_SUF      28
#define LOCALITY    29
#define STATE       30
#define POSTCODE    31
#define ACCESSTYPE  32
#define X           33
#define Y           34

#define NON_STR_DATA 2

/* save the record of a node into output file */
void save_record(FILE *output_file, address_t *data,  char *field_head[]);

/* print the tracked stats during the search into stdout */
void print_stats(search_stats_t *stats, char *target);

#endif 

