
#include "scanner.h"
#include "expression.h"
#include "stackgenerator.h"
#include "symtable.h"
#include "error.h"
#include "code_generator.h"
#include "parser.h"


#define TABLE_SIZE 8

#define FREE_RESOURCES_RETURN(_return)								\
	do {															\
		token_stack_free(&stack);									\
		return _return;												\
	} while(0)


Token_stack stack;
typedef enum
{
    S,    // < SHIFT
    E,    // = EQUAL
    R,    // > REDUCE
    N     // # ERROR
} Prec_table_sign_enum;

typedef enum
{
    INDEX_PLUS_MINUS,		// 0 +-
    INDEX_MUL_DIV,			// */
    INDEX_REL_OP,			// 3 r
    INDEX_LEFT_BRACKET,		// 4 (
    INDEX_RIGHT_BRACKET,	// 5 )
    INDEX_DATA,				// 6 i
    INDEX_DOLLAR			// 7 $
} Prec_table_index_enum;

int prec_table[TABLE_SIZE][TABLE_SIZE] =
        {
            //	|+- | */| \ | r | ( | ) | i | $ |
                { R , S , S , R , S , R , S , R }, // +-
                { R , R , R , R , S , R , S , R }, // */
                { R , S , R , R , S , R , S , R }, // \ /
                { S , S , S , N , S , R , S , R }, // r (realtion operators) = <> <= < >= >
                { S , S , S , S , S , E , S , N }, // (
                { R , R , R , R , N , R , N , R }, // )
                { R , R , R , R , N , R , N , R }, // i (id, int, double, string)
                { S , S , S , S , S , N , S , N }  // $
        };


static Prec_table_index_enum get_prec_table_index(Prec_table_symbol symbol)
{
    switch (symbol)
    {
        case PLUS:
        case MINUS:
            return INDEX_PLUS_MINUS;

        case MUL:
        case DIV:
            return INDEX_MUL_DIV;

        case EQ:
        case NEQ:
        case LSEQ:
        case LSTN:
        case MREQ:
        case MRTN:
            return INDEX_REL_OP;

        case LEFT_BRACKET:
            return INDEX_LEFT_BRACKET;

        case RIGHT_BRACKET:
            return INDEX_RIGHT_BRACKET;

        case IDENTIFIER:
        case INT:
        case FLOAT:
        case STRING:
            return INDEX_DATA;

        default:
            return INDEX_DOLLAR;
    }
}


static Prec_table_symbol get_symbol_from_token(tToken* token)
{
    switch (token->Type)
    {
        case TT_PLUS:
            return PLUS;
        case TT_MINUS:
            return MINUS;
        case TT_MUL:
            return MUL;
        case TT_DIV:
            return DIV;
        case TT_ASSIGN:
            return EQ;
        case TT_NOT_EQUAL:
            return NEQ;
        case TT_LESS_OR_EQUAL:
            return LSEQ;
        case TT_LESS_THAN:
            return LSTN;
        case TT_MORE_OR_EQUAL:
            return MREQ;
        case TT_MORE_THAN:
            return MRTN;
        case TT_LEFT_BRACKET:
            return LEFT_BRACKET;
        case TT_RIGHT_BRACKET:
            return RIGHT_BRACKET;
        case TT_IDENTIFIER:
            return IDENTIFIER;
        case TT_INTEGER:
            return INT;
        case TT_FLOAT:
            return FLOAT;
        case TT_STRING:
            return STRING;
        default:
            return SWAGMONEY;
    }
}

static Data_Type get_data_type(tToken* token, ParserData* data)
{
    TData* symbol;

    switch (token->Type)
    {
        case TT_IDENTIFIER:
            symbol = BST_symtable_Search(&data->local_table, token->Data.string->str);
            if (symbol == NULL)
                return TYPE_NIL;
            return symbol->type;

        case TT_INTEGER:
            return TYPE_INT;

        case TT_FLOAT:
            return TYPE_FLOAT;

        case TT_STRING:
            return TYPE_STRING;
        default:
            return TYPE_NIL;
    }
}

static int num_of_symbols_after_stop(bool* stop_found)
{
    Token_stack_item* tmp = token_stack_top(&stack);
    int count = 0;

    while (tmp != NULL)
    {
        if (tmp->symbol != STOP)
        {
            *stop_found = false;
            count++;
        }
        else
        {
            *stop_found = true;
            break;
        }

        tmp = tmp->next;
    }

    return count;
}

static Prec_rules test_rule(int num, Token_stack_item* op1, Token_stack_item* op2, Token_stack_item* op3)
{
    switch (num)
    {
        case 1:
            // rule E -> i
            if (op1->symbol == IDENTIFIER || op1->symbol == INT || op1->symbol == FLOAT || op1->symbol == STRING)
                return OPERAND;

            return NOT_RULE;

        case 3:
            // rule E -> (E)
            if (op1->symbol == LEFT_BRACKET && op2->symbol == NON_TERMINAL && op3->symbol == RIGHT_BRACKET)
                return LBRCT_E_RBRCT;

            if (op1->symbol == NON_TERMINAL && op3->symbol == NON_TERMINAL)
            {
                switch (op2->symbol)
                {
                    case PLUS:
                        return E_PLUS_E;

                    case MINUS:
                        return E_MINUS_E;

                    case MUL:
                        return E_MUL_E;

                    case DIV:
                        return E_DIV_E;

                    case EQ:
                        return E_EQ_E;

                    case NEQ:
                        return E_NEQ_E;

                    case LSEQ:
                        return E_LSEQ_E;

                    case LSTN:
                        return E_LSTN_E;

                    case MREQ:
                        return E_MREQ_E;

                    case MRTN:
                        return E_MRTN_E;

                    default:
                        return NOT_RULE;
                }
            }
        default:
            return NOT_RULE;
    }
}


static int check_semantics(Prec_rules rule, Token_stack_item* op1, Token_stack_item* op2, Token_stack_item* op3, Data_Type* final_type)
{
    bool retype_op1_to_double = false;
    bool retype_op3_to_double = false;
    if (rule == OPERAND)
    {
        if (op1->data_type == TYPE_NIL)
            return SEMANTIC_ERR_UNDEFINED_VAR;

        if (op1->data_type == TYPE_BOOL)
            return SEMANTIC_ERR_TYPE_COMPAT;
    }

    if (rule == LBRCT_E_RBRCT)
    {
        if (op2->data_type == TYPE_NIL)
            return SEMANTIC_ERR_UNDEFINED_VAR;
    }

    if (rule != OPERAND && rule != LBRCT_E_RBRCT)
    {
        if (op1->data_type == TYPE_NIL || op3->data_type == TYPE_NIL)
            return SEMANTIC_ERR_UNDEFINED_VAR;

        if (op1->data_type == TYPE_BOOL || op3->data_type == TYPE_BOOL)
            return SEMANTIC_ERR_TYPE_COMPAT;
    }

    switch (rule)
    {
        case OPERAND:
            *final_type = op1->data_type;
            break;

        case LBRCT_E_RBRCT:
            *final_type = op2->data_type;
            break;

        case E_PLUS_E:
        case E_MINUS_E:
        case E_MUL_E:
            if (op1->data_type == TYPE_STRING && op3->data_type == TYPE_STRING && rule == E_PLUS_E)
            {
                *final_type = TYPE_STRING;
                break;
            }

            if (op1->data_type == TYPE_INT && op3->data_type == TYPE_INT)
            {
                *final_type = TYPE_INT;
                break;
            }

            if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
                return SEMANTIC_ERR_TYPE_COMPAT;

            *final_type = TYPE_FLOAT;

            if (op1->data_type == TYPE_INT)
                retype_op1_to_double = true;

            if (op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            break;

        case E_DIV_E:
            *final_type = TYPE_FLOAT;

            if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
                return SEMANTIC_ERR_TYPE_COMPAT;

            if (op1->data_type == TYPE_INT)
                retype_op1_to_double = true;

            if (op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            break;

        case E_EQ_E:
        case E_NEQ_E:
        case E_LSEQ_E:
        case E_LSTN_E:
        case E_MREQ_E:
        case E_MRTN_E:
            *final_type = TYPE_BOOL;

            if (op1->data_type == TYPE_INT && op3->data_type == TYPE_FLOAT)
                retype_op1_to_double = true;

            else if (op1->data_type == TYPE_FLOAT && op3->data_type == TYPE_INT)
                retype_op3_to_double = true;

            else if (op1->data_type != op3->data_type)
                return SEMANTIC_ERR_TYPE_COMPAT;

            break;

        default:
            break;
    }

    if (retype_op1_to_double)
    {
        GENERATE_CODE(generate_stack_op2_to_float);
    }

    if (retype_op3_to_double)
    {
        GENERATE_CODE(generate_stack_op1_to_float);
    }
    return SYNTAX_OK;
}

static int reduce_by_rule(ParserData* data)
{
    (void) data;

    int result;

    Token_stack_item* op1 = NULL;
    Token_stack_item* op2 = NULL;
    Token_stack_item* op3 = NULL;
    Data_Type final_type;
    Prec_rules rule_for_code_gen;
    bool found = false;

    int count = num_of_symbols_after_stop(&found);


    if (count == 1 && found)
    {
        op1 = stack.top;
        rule_for_code_gen = test_rule(count, op1, NULL, NULL);
    }
    else if (count == 3 && found)
    {
        op1 = stack.top->next->next;
        op2 = stack.top->next;
        op3 = stack.top;
        rule_for_code_gen = test_rule(count, op1, op2, op3);
    }
    else
        return SYNTAX_ERR;

    if (rule_for_code_gen == NOT_RULE)
    {
        return SYNTAX_ERR;
    }
    else
    {
        if ((result = check_semantics(rule_for_code_gen, op1, op2, op3, &final_type)))
            return result;

        if (rule_for_code_gen == E_PLUS_E && final_type == TYPE_STRING)
        {
            GENERATE_CODE(generate_concat_stack_strings);
        }
        else GENERATE_CODE(generate_stack_operation, rule_for_code_gen);

        token_stack_pull_count(&stack, count + 1);
        token_stack_push(&stack, NON_TERMINAL, final_type);
    }

    return SYNTAX_OK;
}


int expression(ParserData* data)
{
    int result = SYNTAX_ERR;

    token_stack_init(&stack);

    if (!token_stack_push(&stack, SWAGMONEY, TYPE_NIL))
        FREE_RESOURCES_RETURN(ERROR_INTERNAL);

    Token_stack_item* top_stack_terminal;
    Prec_table_symbol actual_symbol;

    bool success = false;

    do
    {
        actual_symbol = get_symbol_from_token(&data->token);
        top_stack_terminal = token_stack_top_terminal(&stack);

        if (top_stack_terminal == NULL)
            FREE_RESOURCES_RETURN(ERROR_INTERNAL);

        switch (prec_table[get_prec_table_index(top_stack_terminal->symbol)][get_prec_table_index(actual_symbol)])
        {
            case S:
                if (!token_stack_insert_after_top_terminal(&stack, STOP, TYPE_NIL))
                    FREE_RESOURCES_RETURN(ERROR_INTERNAL);

                if(!token_stack_push(&stack, actual_symbol, get_data_type(&data->token, data)))
                    FREE_RESOURCES_RETURN(ERROR_INTERNAL);

                if (actual_symbol == IDENTIFIER || actual_symbol == INT || actual_symbol == FLOAT || actual_symbol == STRING)
                {
                    GENERATE_CODE(generate_push, data->token);
                }

                if ((result = getToken(&data->token)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case E:
                token_stack_push(&stack, actual_symbol, get_data_type(&data->token, data));

                if ((result = getToken(&data->token)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case R:
                if ((result = reduce_by_rule(data)))
                    FREE_RESOURCES_RETURN(result);
                break;

            case N:
                if (actual_symbol == SWAGMONEY && top_stack_terminal->symbol == SWAGMONEY)
                    success = true;
                else
                    FREE_RESOURCES_RETURN(SYNTAX_ERR);
                break;
            default:SYNTAX_OK;
        }
    } while (!success);

    Token_stack_item* final_non_terminal = token_stack_top(&stack);
    if (final_non_terminal == NULL)
        FREE_RESOURCES_RETURN(ERROR_INTERNAL);
    if (final_non_terminal->symbol != NON_TERMINAL)
        FREE_RESOURCES_RETURN(SYNTAX_ERR);

    if (data->ls_id != NULL)
    {
        char *frame = "LF";
        if (data->ls_id->globalvar) frame = "GF";

        switch (data->ls_id->type)
        {
            case TYPE_INT:
                if (final_non_terminal->data_type == TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEMANTIC_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->ls_id->identifier, final_non_terminal->data_type, TYPE_INT, frame);
                break;

            case TYPE_FLOAT:
                if (final_non_terminal->data_type == TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEMANTIC_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->ls_id->identifier, final_non_terminal->data_type, TYPE_FLOAT, frame);
                break;

            case TYPE_STRING:
                if (final_non_terminal->data_type != TYPE_STRING)
                    FREE_RESOURCES_RETURN(SEMANTIC_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->ls_id->identifier, TYPE_STRING, TYPE_STRING, frame);
                break;

            case TYPE_NIL:
                GENERATE_CODE(generate_save_expression_result, data->ls_id->identifier, final_non_terminal->data_type, TYPE_NIL, frame);
                break;

            case TYPE_BOOL:
                if (final_non_terminal->data_type != TYPE_BOOL)
                    FREE_RESOURCES_RETURN(SEMANTIC_ERR_TYPE_COMPAT);

                GENERATE_CODE(generate_save_expression_result, data->ls_id->identifier, final_non_terminal->data_type, TYPE_BOOL, frame);
                break;

            default:
                break;
        }
    }

    FREE_RESOURCES_RETURN(SYNTAX_OK);
}
