//
// Created by Selenmon on 20.11.2018.
//

#ifndef IFJ18_EXPRESSION_H
#define IFJ18_EXPRESSION_H

#include "parser.h"
typedef enum
{
    E_EQ_E,         // E-> E = E
    E_NEQ_E,        // E-> E != E
    E_LSEQ_E,       // E-> E <= E
    E_LSTN_E,       // E-> E < E
    E_MREQ_E,       // E-> E => E
    E_MRTN_E,       // E-> E > E
    E_PLUS_E,       // E-> E + E
    E_MINUS_E,      // E-> E - E
    E_MUL_E,        // E-> E / E
    E_DIV_E,        // E-> E * E
    LBRCT_E_RBRCT,  // E-> (E)
    OPERAND,        // E-> i
    NOT_RULE        // rule not relevant

} Prec_rules;




typedef enum
{
    PLUS,                    // +
    MINUS,                   // -
    MUL,                     // *
    DIV,                     // /
    EQ,                      // ==
    NEQ,                     // !=
    LSEQ,                    // <=
    LSTN,                    // <
    MREQ,                    // >=
    MRTN,                    // >
    LEFT_BRACKET,            // (
    RIGHT_BRACKET,            // )
    IDENTIFIER,              // ID
    INT,                     // int
    FLOAT,                   // float
    STRING,                  // string
    SWAGMONEY,               // $
    STOP,                    // stop for reducing
    NON_TERMINAL             // its self explanatory
} Prec_table_symbol;

#endif //IFJ18_EXPRESSION_H
