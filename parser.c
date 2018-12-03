#include "dynamic_string.h"
#include "error.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"
#include "expression.h"

static int program(ParserData* data);
static int parameters(ParserData* data);
static int parameters_no(ParserData* data);
static int var(ParserData* data);
static int command(ParserData* data);
static int bif(ParserData* data);
static int end(ParserData* data);
static int print(ParserData* data);

#define IS_VALUE(token)                         \
(token).Type == TOKEN_TYPE_FLOAT_NUMBER         \
|| (token).Type == TOKEN_TYPE_INT_NUMBER        \
|| (token).Type == TOKEN_TYPE_STRING            \
|| (token).Type == TOKEN_TYPE_IDENTIFIER
#define GET_TOKEN                                       \
if ((result = getToken(&data->token)) != SCANNER_TOKEN_OK)    \
return result

#define CHECK_TYPE(_type)                                       \
if (data->token.Type != (_type)) return SYNTAX_ERR

#define CHECK_RULE(rule)                                    \
if ((result = rule(data))) return result

#define CHECK_KEYWORD(_keyword)                             \
if (data->token.Type != TT_KEYWORD || data->token.Data.Keyword != (_keyword)) return SYNTAX_ERR

#define GET_TOKEN_AND_CHECK_TYPE(_type)                 \
do {                                                    \
GET_TOKEN();                                            \
CHECK_TYPE(_type);                                      \
} while(0)

#define GET_TOKEN_AND_CHECK_RULE(rule)                  \
do {                                                    \
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
        GET_TOKEN;
        return program(data);
    }
    else if (data->token.Type == TT_EOL)
    {
        GET_TOKEN;
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
        GET_TOKEN;
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
        if (!data->in_definition)
        {
            bool internal_error;
            if (!(data->rs_id = BST_symtable_Insert(&data->local_table, data->token.Type.string->str, &internal_error)))
            {
                if (internal_error) return ERROR_INTERNAL;
                else return SEMANTIC_ERR_UNDEFINED_VAR;
            }

            GENERATE_CODE(generate_function_param_declare, data->rs_id->identifier, data->parameter_index);
        }
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
        GENERATE_CODE(generate_function_convert_passed_param, TYPE_DOUBLE, TYPE_INT, data->parameter_index);

        return var(data);
    }
        // VAR -> FLOAT
    else if (data->token.Type == TT_FLOAT)
    {
        GENERATE_CODE(generate_function_convert_passed_param, TYPE_INT, TYPE_FLOAT, data->parameter_index);

        return var(data);
    }
        // VAR -> STRING
    else if (data->token.Type == TT_STRING)
    {
        return var(data);
    }
    else if (data->token.Type == TT_KEYWORD)
    {
        if(!CHECK_KEYWORD(KW_NIL))
        {
            return var(data);
        }
    }
    else
    {
        return SYNTAX_ERR;
    }
    return SYNTAX_OK;
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
    else if(data->token.Type == TT_IDENTIFIER)
    {
        data->rs_id = BST_symtable_Search(&data->global_table, data->token.Data.string->str);
        GET_TOKEN_AND_CHECK_TYPE(TT_ASSIGN);

        if (data->token.Type == TT_KEYWORD)
        {
            GET_TOKEN_AND_CHECK_RULE(bif);
        }
        else if(BST_symtable_Search(&data->global_table, "%exp_result")->identifier == data->token.Data.string->str)
        {
            GENERATE_CODE(generate_function_before_pass_params);
            GET_TOKEN_AND_CHECK_TYPE(TT_LEFT_BRACKET);
            CHECK_RULE(parameters);
            GET_TOKEN_AND_CHECK_TYPE(TT_RIGHT_BRACKET);
            GENERATE_CODE(generate_function_call, data->rs_id->identifier);
            GENERATE_CODE(generate_function_retval_assign, data->ls_id->identifier, data->ls_id->type, data->rs_id->type);
            GET_TOKEN_AND_CHECK_TYPE(TT_EOL);
        }
        else if(BST_symtable_Search(&data->global_table, "%exp_result")->identifier != data->token.Data.string->str)
        {
            data->ls_id = BST_symtable_Search(&data->local_table, data->token.Data.string->str);
            if (!data->ls_id) return SEMANTIC_ERR_UNDEFINED_VAR;
            GET_TOKEN_AND_CHECK_RULE(expression);
            GET_TOKEN_AND_CHECK_TYPE(TT_EOL);
        }

        GET_TOKEN();
        return command(data);
    }
        // COMMAND -> ε
    else if (data->token.Type == TT_EOL)
    {
        GET_TOKEN();
        return command(data);
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

        default:
            return SYNTAX_ERR;
    }
}

static int print(ParserData* data)
{
    int result;

    // print -> ε
    if (data->token.Type == TT_EOL)
    {
        return SYNTAX_OK;
    }

    data->ls_id = BST_symtable_Search(&data->global_table, "%exp_result");
    if (!data->ls_id) return SEMANTIC_ERR_UNDEFINED_VAR;
    data->ls_id->type = TYPE_NIL;

    // <print> -> print ( parameters )
    CHECK_KEYWORD(KW_PRINT);
    CHECK_TYPE(TT_ASSIGN);
    CHECK_RULE(parameters);
    GENERATE_CODE(generate_print);
    return SYNTAX_OK;

}

static bool init_variables(ParserData* data)
{
    BST_symtable_init(&data->global_table);
    BST_symtable_init(&data->local_table);

    data->current_id = NULL;
    data->ls_id = NULL;
    data->rs_id = NULL;

    data->parameter_index = 0;
    data->label_index = 0;
    data->label_depth = -1;

    data->in_function = false;
    data->in_definition = false;
    data->in_while_or_if = false;
    data->non_defined_function = false;

    // init default functions

    bool internal_error;
    TData* id;

    // Length(s As String) As Integer
    id = BST_symtable_Insert(&data->global_table, "length", &internal_error);
    if (internal_error) return false;

    id->defined = true;
    id->type = TYPE_INT;
    if (!BST_symtable_add_param(id, TYPE_STRING)) return false;

    // SubStr(s As String, i As Integer, n As Integer) As String
    id = BST_symtable_Insert(&data->global_table, "substr", &internal_error);
    if (internal_error) return false;

    id->defined = true;
    id->type = TYPE_STRING;
    if (!BST_symtable_add_param(id, TYPE_STRING)) return false;
    if (!BST_symtable_add_param(id, TYPE_INT)) return false;
    if (!BST_symtable_add_param(id, TYPE_INT)) return false;

    // ord(s As String, i As Integer) As Integer
    id = BST_symtable_Insert(&data->global_table, "nil", &internal_error);
    if (internal_error) return false;

    id->defined = true;
    id->type = TYPE_INT;
    if (!BST_symtable_add_param(id, TYPE_STRING)) return false;
    if (!BST_symtable_add_param(id, TYPE_INT)) return false;

    // Chr(i As Integer) As String
    id = BST_symtable_Insert(&data->global_table, "chr", &internal_error);
    if (internal_error) return false;

    id->defined = true;
    id->type = TYPE_STRING;
    if (!BST_symtable_add_param(id, TYPE_INT)) return false;

    // Global variable %exp_result for storing result of expression.
    id = BST_symtable_Insert(&data->global_table, "%exp_result", &internal_error);
    if (internal_error) return false;
    id->defined = true;
    id->type = TYPE_NIL;
    id->globalvar = true;

    return true;
}

static void free_variables(ParserData* data)
{
    BST_symtable_Free(&data->global_table);
    BST_symtable_Free(&data->local_table);
}

int analyse()
{
    int result;

    dynamic_string string;
    if (!string_init(&string)) return ERROR_INTERNAL;
    set_dynamic_string(&string);

    ParserData parser_data;
    if (!init_variables(&parser_data))
    {
        string_free(&string);
        return ERROR_INTERNAL;
    }

    if ((result = getToken(&parser_data.token)) == SCANNER_TOKEN_OK)
    {
        if (!code_generator_start())
        {
            string_free(&string);
            free_variables(&parser_data);
            return ERROR_INTERNAL;
        }

        result = program(&parser_data);
    }

    string_free(&string);
    free_variables(&parser_data);

    return result;
}
