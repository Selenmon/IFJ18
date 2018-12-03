//
// Created by Selenmon on 20.11.2018.
//
#include "stackgenerator.h"

#include <stdio.h>
#include <stdlib.h>

#include "stackgenerator.h"

void token_stack_init(Token_stack* stack)
{
    stack->top =  NULL;
}

bool token_stack_push(Token_stack* stack, Prec_table_symbol symbol, Data_Type type)
{
    Token_stack_item* new_item = (Token_stack_item*)malloc(sizeof(Token_stack_item));

    if (new_item == NULL)
        return false;
    new_item->symbol = symbol;
    new_item->data_type = type;
    new_item->next = stack->top;

    stack->top = new_item;

    return true;
}

bool token_stack_pull(Token_stack* stack)
{
    if (stack->top != NULL)
    {
        Token_stack_item* tmp = stack->top;
        stack->top = tmp->next;
        free(tmp);

        return true;
    }
    else
    {
        return false;
    }
}

void token_stack_pull_count(Token_stack* stack, int amount)
{
    for (int i = 0; i < amount ; i++)
    {
        token_stack_pull(stack);
    }
}

Token_stack_item* token_stack_top_terminal(Token_stack* stack)
{
    for (Token_stack_item* tmp = stack->top; tmp != NULL; tmp = tmp->next)
    {
        if(tmp->symbol < STOP)
            return tmp;
    }
}

bool token_stack_insert_after_top_terminal(Token_stack* stack, Prec_table_symbol symbol, Data_Type type)
{
    Token_stack_item* previous = NULL;

    for (Token_stack_item* tmp = stack->top; tmp != NULL; tmp->next)
    {
        if (tmp->symbol < STOP)
        {
            Token_stack_item* new_item = (Token_stack_item*)malloc(sizeof(Token_stack_item));

            if (new_item == NULL)
                return false;
            new_item->symbol =symbol;
            new_item->data_type = type;

            if(previous == NULL)
            {
                new_item->next = stack->top;
                new_item->data_type = type;
            }
            else
            {
                new_item->next = previous->next;
                previous->next = new_item;
            }

            return true;
        }
        previous = tmp;
    }
    return false;
}

Token_stack_item* token_stack_top(Token_stack* stack)
{
    return  stack->top;
}

void token_stack_free(Token_stack* stack)
{
    while(token_stack_pull(stack));
}