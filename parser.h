#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "BSTsymtable.h"


typedef struct
{
    struct tBSTsymtable global_table;
    struct tBSTsymtable local_table;
    tToken token;
    TData* current_id;
    TData* ls_id;
    TData* rs_id;

    unsigned param_index;
    int label_index;
    int label_depth;

    bool in_function;
    bool in_declaration;
    bool in_while_or_if;
    bool non_declared_function;
} ParserData;

int analyse();

#endif // PARSER_H