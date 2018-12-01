#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "bstsymtable.h"

#define GENERATE_CODE(_callback, ...)								\
	if (!_callback(__VA_ARGS__)) return ERROR_INTERNAL

typedef struct
{
    struct tBSTsymtable global_table;
    struct tBSTsymtable local_table;
    tToken token;
    TData* current_id;
    TData* ls_id;
    TData* rs_id;

    unsigned parameter_index;
    int label_index;
    int label_depth;

    bool in_function;
    bool in_definition;
    bool in_while_or_if;
    bool non_defined_function;
} ParserData;

int analyse();

#endif // PARSER_H