//
// Created by Selenmon on 20.11.2018.
//
#ifndef TOKEN_STACK_LIBRARY_H
#define TOKEN_STACK_LIBRARY_H

#include <stdbool.h>

#include "expression.h"
#include "BSTsymtable.h"


typedef struct stack_item
{
    Potential_symbol symbol;
    Data_Type data_type;
    struct stack_item *next;
} Token_stack_item;

typedef struct stack
{
    Token_stack_item *top;
} Token_stack;

/**
 * Function intializes stack
 *
 * @param stack Pointer to stack
 */
void token_stack_init(Token_stack* stack);

/**
 * Function pushes token to stack and sets the data type
 *
 * @param stack Pointer to stack
 * @param symbol Symbol of token
 * @param type Data type of token
 * @return True if successful else false
 */
bool token_stack_push(Token_stack* stack, Potential_symbol symbol, Data_Type type);

/**
 * @brief Function pulls top token from stack
 *
 * @param stack Pointer top stack
 * @return True if successful else false
 */
bool token_stack_pull(Token_stack* stack);

/**
 * @brief Function pulls from stack multiple times
 *
 * @param stack Pointer to stack
 * @param amount Amount of stacks about to be pulled
 */
void token_stack_pull_count(Token_stack* stack, int amount);

/**
 * @brief Function returns top terminal from stack
 *
 * @param stack Pointer to stack
 * @return Pointer to to terminal
 */
Token_stack_item* token_stack_top_terminal(Token_stack* stack);

/**
 * @brief Function inserts token after top terminal
 *
 * @param stack Pointer to stack
 * @param symbol Symbol of token
 * @param type Data type of token
 * @return True if successful else false
 */
bool token_stack_insert_after_top_terminal(Token_stack* stack, Potential_symbol symbol, Data_Type type);

/**
 * @brief Function returns top symbol of stack
 *
 * @param stack Pointer to stack
 * @return Pointer to token on top of stack
 */
Token_stack_item* token_stack_top(Token_stack* stack);

/**
 * @brief Function frees all allocated resources for stack
 *
 * @param stack Pointer to stack
 */
void token_stack_free(Token_stack* stack);
#endif