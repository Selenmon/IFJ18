#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "symtable.h"


typedef struct
{

    Token token;                //token
    TData* current_id;          //ID of processed function
    TData* ls_id;               //ID of left-side variable
    TData* rs_id;               //ID of right-side function or expression

    unsigned param_index;       //Index of current param
    int label_index;            //Index for generating unique labels
    int label_depth;            //Depth of labels

    bool in_function;           // is 1 if parser is in function
    bool in_declaration;        // is 1 if param rule should add or check
    bool in_while_or_if;        // is 1 if parser is in construction while, if or then
    bool non_declared_function;  // is 1 if parser is in function only defined
};

int analyse();

#endif // PARSER_H