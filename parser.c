#include "dynamic_string.h"
#include "error.h"
#include "bstsymtable.h"
#include "parser.h"
#include "code_generator.h"

static int program(ParserData* data);
static int parameters(ParserData* data);
static int parameters_no(ParserData* data);
static int var(ParserData* data);
static int command(ParserData* data);
static int bif(ParserData* data);
static int end(ParserData* data);

#define IS_VALUE(token)                         \
(token).Type == TOKEN_TYPE_FLOAT_NUMBER         \
|| (token).Type == TOKEN_TYPE_INT_NUMBER        \
|| (token).Type == TOKEN_TYPE_STRING            \
|| (token).Type == TOKEN_TYPE_IDENTIFIER

#define GET_TOKEN ()                                        \
if ((result = get_next_token(&data->token)) != SCANNER_TOKEN_OK)    \
return result

#define CHECK_TYPE(_type)                                       \
if (data->token.Type != (_type)) return SYNTAX_ERR

#define CHECK_RULE(rule)                                    \
if ((result = rule(data))) return result

#define CHECK_KEYWORD(_keyword)                             \
if (                                            \
data->token.Type != TT_KEYWORD                  \
|| data->token.Data.Keyword != (_keyword)          \
) return SYNTAX_ERR

#define GET_TOKEN_AND_CHECK_TYPE(_type)                 \
do {                                            \
GET_TOKEN();                                            \
CHECK_TYPE(_type);                                      \
} while(0)

#define GET_TOKEN_AND_CHECK_RULE(rule)                  \
do {                                            \
GET_TOKEN();                                            \
CHECK_RULE(rule);                                       \
} while(0)

#define GET_TOKEN_AND_CHECK_KEYWORD(_keyword)       \
do {                                            \
GET_TOKEN();                                        \
CHECK_KEYWORD(_keyword);                            \
} while(0)

static int program(ParserData* data)
{
    int result;

    // PROGRAM -> DEF ID/VAR ( PARAMETERS ) EOL COMMAND END
    if (data->token.Type == TT_KEYWORD && data->token.Data.Keyword == KW_DEF)
    {
        data->in_definition = true;
        data->label_index = 0;

        GET_TOKEN_AND_CHECK_TYPE(TT_IDENTIFIER);
        GENERATE_CODE(generate_function_start, data->current_id->identifier);
        CHECK_RULE(var);
        GENERATE_CODE(generate_function_retval, data->current_id->type);
        GET_TOKEN_AND_CHECK_TYPE(TT_LEFT_BRACKET);

        data->current_id = BST_symtable_Insert(&data->global_table, data->token.Data.string->str, ERROR_INTERNAL);

        GET_TOKEN_AND_CHECK_RULE(parameters);
        GET_TOKEN_AND_CHECK_TYPE(TT_RIGHT_BRACKET);
        GET_TOKEN_AND_CHECK_TYPE(TT_EOL);
        GET_TOKEN_AND_CHECK_RULE(command);
        CHECK_KEYWORD(KW_END);
        GENERATE_CODE(generate_function_end, data->current_id->identifier);

        data->current_id->defined = true;
        BST_symtable_Free(&data->local_table);

        // NEXT
        GET_TOKEN();
        return program(data);
    }
    else if (data->token.Type == TT_EOL)
    {
        GET_TOKEN();
        return program(data);
    }

    return SYNTAX_ERR;
}

static int end(ParserData* data)
{
    int result;

    // PROGRAM -> EOL <end>
    if (data->token.Type == TT_EOL)
    {
        GET_TOKEN();
        return end(data);
    }

    // END -> EOF
    if (data->token.Type == TT_EOF)
    {
        return SYNTAX_OK;
    }

    return SYNTAX_ERR;
}

static int parameters(ParserData* data)
{
    int result;
    data->parameter_index = 0;

    // PARAMETERS -> VAR PARAMETERS_NO
    if(CHECK_RULE(var) == var)
    {
        CHECK_RULE(parameters_no);
        if (data->parameter_index + 1 != data->current_id->params->len)
            return SEMANTIC_ERR_UNDEFINED_VAR;
    }
    return SYNTAX_OK;
}

static int parameters_no(ParserData* data)
{
    int result;

    // PARAMETERS_NO -> , VAR PARAMETERS_NO
    if (data->token.Type == TT_COMMA)
    {
        data->parameter_index++;

        CHECK_RULE(var);
        CHECK_RULE(parameters_no);
        GET_TOKEN();
        return parameters_no(data);

    }
    return SYNTAX_OK;
}

static int var(ParserData* data)
{
    // VAR -> INT
    if (data->token.Type == TT_INTEGER)
    {
        //check somethingorother
    }
        // VAR -> FLOAT
    else if (data->token.Type == TT_FLOAT)
    {
        //check somethingorother
    }
        // VAR -> STRING
    else if (data->token.Type == TT_STRING)
    {
        //check somethingorother
    }
}

static int command(ParserData* data)
{
    int result;

    // COMMAND -> IF EXPR THEN EOL COMMAND END
    if(data->token.Type == TT_KEYWORD && data->token.Data.Keyword == KW_IF)
    {
        data->label_depth++;
        data->in_while_or_if = true;

        data->ls_id = BST_symtable_Search(&data->global_table, "%exp_result");
        if (!data->ls_id) return SEMANTIC_ERR_UNDEFINED_VAR;
        data->ls_id->type = TYPE_BOOL;

        char *function_id = data->current_id ? data->current_id->identifier : "";
        int current_label_index = data->label_index;
        data->label_index += 2;

        GENERATE_CODE(generate_if_head);

        GET_TOKEN_AND_CHECK_RULE(expression);

        CHECK_KEYWORD(KW_THEN);

        GENERATE_CODE(generate_if_start, function_id, current_label_index, data->label_depth);

        GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

        GET_TOKEN_AND_CHECK_RULE(command);

        CHECK_KEYWORD(KW_END);

        GENERATE_CODE(generate_if_end, function_id, current_label_index + 1, data->label_depth);

        data->label_depth--;
        data->in_while_or_if = false;

        GET_TOKEN();
        return command(data);
    }
        // COMMAND -> WHILE EXPR DO EOL COMMAND END
    else if(data->token.Type == TT_KEYWORD && data->token.Data.Keyword == KW_WHILE)
    {
        data->ls_id = BST_symtable_Search(&data->global_table, "%exp_result");
        if (!data->ls_id) return SEMANTIC_ERR_UNDEFINED_VAR;
        data->ls_id->type = TYPE_BOOL;

        char *function_id = data->current_id ? data->current_id->identifier : "";
        int current_label_index = data->label_index;
        data->label_index += 2;

        GENERATE_CODE(generate_while_head, function_id, current_label_index, data->label_depth);

        GET_TOKEN_AND_CHECK_RULE(expression);

        CHECK_KEYWORD(KW_DO);

        GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

        GENERATE_CODE(generate_while_start, function_id, current_label_index + 1, data->label_depth);

        GET_TOKEN_AND_CHECK_RULE(command);

        CHECK_KEYWORD(KW_END);

        GENERATE_CODE(generate_while_end, function_id, current_label_index + 1, data->label_depth);

        data->label_depth--;
        data->in_while_or_if = false;

        GET_TOKEN();
        return command(data);
    }
    // COMMAND -> ID = EXPR EOL /  ID = FUNCTION EOL
    if(data->token.Type == TT_IDENTIFIER)
    {
        GET_TOKEN_AND_CHECK_TYPE(TT_ASSIGN);

        if(EXPR IS A FUNCTION TODO)
        {
            GET_TOKEN_AND_CHECK_RULE(function);
        }
        else if(EXPR IS NOT A FUNCTION TODO)
        {
            GET_TOKEN_AND_CHECK_RULE(expr);
        }

        GET_TOKEN();
        return statement(data);
    }
        // COMMAND -> ε
    else if (data->token.type == TOKEN_TYPE_EOL)
    {
        GET_TOKEN();
        return statement(data);
    }
}

static int bif(ParserData* data)
{
    switch(data->token.Type == TT_KEYWORD)
    {
        // BIF -> LENGTH ( PARAMETERS )
        case KW_LENGTH:
            data->rs_id = BST_symtable_Search(&data->global_table, "length");
            break;

            // BIF -> SUBSTR ( PARAMETERS )
        case KW_SUBSTR:
            data->rs_id = BST_symtable_Search(&data->global_table, "substr");
            break;

            // BIF -> NIL ( PARAMETERS )
        case KW_NIL:
            data->rs_id = BST_symtable_Search(&data->global_table, "nil");
            break;
            // BIF -> ORD ( PARAMETERS )
        case KW_ORD:
            data->rs_id = BST_symtable_Search(&data->global_table, "ord");
            break;
            // BIF -> CHR ( PARAMETERS )
        case KW_CHR:
            data->rs_id = BST_symtable_Search(&data->global_table, "chr");
            break;
    }
}