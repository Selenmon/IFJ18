#include <ctype.h>

#include "dynamic_string.h"
#include "scanner.h"
#include "code_generator.h"

#define ADD_INST(_inst) if (!string_add_const(&code,(_inst "\n"))) return false

#define ADD_CODE(_code) if (!string_add_const(&code, (_code))) return false

#define ADD_CODE_INT(_code) do{ char str[MAX_DIG]; sprintf(str, "%d", _code); ADD_CODE(str);} while (0)

#define MAX_DIG 50

#define FUNCTION_INPUT_S                                                                            \
    "\n # Built-in function Inputs"                                                                 \
    "\n LABEL $inputs"                                                                              \
    "\n PUSHFRAME"                                                                                  \
    "\n DEFVAR LF@%retval"                                                                          \
    "\n READ LF@%retval string"                                                                     \
    "\n POPFRAME"                                                                                   \
    "\n RETURN"

#define FUNCTION_INPUT_I                                                                            \
    "\n # Built-in function Inputi"                                                                 \
    "\n LABEL $inputi"                                                                              \
    "\n PUSHFRAME"                                                                                  \
    "\n DEFVAR LF@%iint"                                                                            \
    "\n READ LF@%iint int"                                                                          \
    "\n POPFRAME"                                                                                   \
    "\n RETURN"


#define FUNCTION_INPUT_F                                                                            \
    "\n # Built-in function Inputf"                                                                 \
    "\n LABEL $inputf"                                                                              \
    "\n PUSHFRAME"                                                                                  \
    "\n DEFVAR LF@%retval"                                                                          \
    "\n READ LF@%retval float"                                                                      \
    "\n POPFRAME"                                                                                   \
    "\n RETURN"

#define FUNCTION_PRINT                                                                              \
    "\n # Built-in function Print"                                                                  \
    "\n LABEL $print"                                                                               \
    "\n PUSHFRAME"                                                                                  \
    "\n DEFVAR LF@%printval"                                                                        \
    "\n MOVE LF@%printval string@"                                                                  \
    "\n WRITE LF@%printval"                                                                         \
    "\n POPFRAME"                                                                                   \
    "\n RETURN"

#define FUNCTION_LENGTH														                        \
	"\n # Built-in function Length"											                        \
	"\n LABEL $length"														                        \
	"\n PUSHFRAME"															                        \
	"\n DEFVAR LF@%retval"													                        \
	"\n STRLEN LF@%retval LF@%0"											                        \
	"\n POPFRAME"															                        \
	"\n RETURN"

// SubStr(s As String, i As Integer, n As Integer) As String
#define FUNCTION_SUBSTR														                        \
	"\n # Built-in function SubStr"											                        \
	"\n LABEL $substr"														                        \
	"\n PUSHFRAME"															                        \
	"\n DEFVAR LF@%retval"													                        \
	"\n MOVE LF@%retval string@"											                        \
	"\n DEFVAR LF@length_str"												                        \
	"\n CREATEFRAME"														                        \
	"\n DEFVAR TF@%0"														                        \
	"\n MOVE TF@%0 LF@%0"													                        \
	"\n CALL $length"														                        \
	"\n MOVE LF@length_str TF@%retval"										                        \
	"\n DEFVAR LF@ret_cond"													                        \
	"\n LT LF@ret_cond LF@length_str int@0"									                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n EQ LF@ret_cond LF@length_str int@0"									                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n LT LF@ret_cond LF@%1 int@0"											                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n EQ LF@ret_cond LF@%1 int@0"											                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n GT LF@ret_cond LF@%1 LF@length_str"									                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n EQ LF@ret_cond LF@%2 int@0"											                        \
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						                        \
	"\n DEFVAR LF@max_n"													                        \
	"\n MOVE LF@max_n LF@length_str"										                        \
	"\n SUB LF@max_n LF@max_n LF@%1"										                        \
	"\n ADD LF@max_n LF@max_n int@1"										                        \
	"\n DEFVAR LF@edit_n_cond"												                        \
	"\n LT LF@edit_n_cond LF@%2 int@0"										                        \
	"\n JUMPIFEQ $substr$edit_n LF@edit_n_cond bool@true"					                        \
	"\n GT LF@edit_n_cond LF@%2 LF@max_n"									                        \
	"\n JUMPIFEQ $substr$edit_n LF@edit_n_cond bool@true"					                        \
	"\n JUMP $substr$process"												                        \
	"\n LABEL $substr$edit_n"												                        \
	"\n MOVE LF@%2 LF@max_n"												                        \
	"\n LABEL $substr$process"												                        \
	"\n DEFVAR LF@index"													                        \
	"\n MOVE LF@index LF@%1"												                        \
	"\n SUB LF@index LF@index int@1"										                        \
	"\n DEFVAR LF@char"														                        \
	"\n DEFVAR LF@process_loop_cond"										                        \
	"\n LABEL $substr$process_loop"											                        \
	"\n GETCHAR LF@char LF@%0 LF@index"										                        \
	"\n CONCAT LF@%retval LF@%retval LF@char"								                        \
	"\n ADD LF@index LF@index int@1"										                        \
	"\n SUB LF@%2 LF@%2 int@1"												                        \
	"\n GT LF@process_loop_cond LF@%2 int@0"								                        \
	"\n JUMPIFEQ $substr$process_loop LF@process_loop_cond bool@true"		                        \
	"\n LABEL $substr$return"												                        \
	"\n POPFRAME"															                        \
	"\n RETURN"

// Ord(s As String, i As Integer) As Integer
#define FUNCTION_ORD														                        \
	"\n # Built-in function Ord"											                        \
	"\n LABEL $asc"															                        \
	"\n PUSHFRAME"															                        \
	"\n DEFVAR LF@%retval"													                        \
	"\n MOVE LF@%retval int@0"												                        \
	"\n DEFVAR LF@cond_length"												                        \
	"\n LT LF@cond_length LF@%1 int@1"										                        \
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						                        \
	"\n DEFVAR LF@length_str"												                        \
	"\n CREATEFRAME"														                        \
	"\n DEFVAR TF@%0"														                        \
	"\n MOVE TF@%0 LF@%0"													                        \
	"\n CALL $length"														                        \
	"\n MOVE LF@length_str TF@%retval"										                        \
	"\n GT LF@cond_length LF@%1 LF@length_str"								                        \
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						                        \
	"\n SUB LF@%1 LF@%1 int@1"												                        \
	"\n STRI2INT LF@%retval LF@%0 LF@%1"									                        \
	"\n LABEL $asc$return"													                        \
	"\n POPFRAME"															                        \
	"\n RETURN"

// Chr(i As Integer) As String
#define FUNCTION_CHR														                        \
	"\n # Built-in function Chr"											                        \
	"\n LABEL $chr"															                        \
	"\n PUSHFRAME"															                        \
	"\n DEFVAR LF@%retval"													                        \
	"\n MOVE LF@%retval string@"											                        \
	"\n DEFVAR LF@cond_range"												                        \
	"\n LT LF@cond_range LF@%0 int@0"										                        \
	"\n JUMPIFEQ $chr$return LF@cond_range bool@true"						                        \
	"\n GT LF@cond_range LF@%0 int@255"										                        \
	"\n JUMPIFEQ $chr$return LF@cond_range bool@true"						                        \
	"\n INT2CHAR LF@%retval LF@%0"											                        \
	"\n LABEL $chr$return"													                        \
	"\n POPFRAME"															                        \
	"\n RETURN"

dynamic_string code;

static bool define_built_in_functions()
{
    ADD_INST(FUNCTION_LENGTH);
    ADD_INST(FUNCTION_SUBSTR);
    ADD_INST(FUNCTION_ORD);
    ADD_INST(FUNCTION_CHR);
    ADD_INST(FUNCTION_INPUT_S);
    ADD_INST(FUNCTION_INPUT_I);
    ADD_INST(FUNCTION_INPUT_F);
    ADD_INST(FUNCTION_PRINT);

    return true;
}

static bool generate_file_header()
{
    ADD_INST("# Start of program");
    ADD_INST(".IFJcode18");
    ADD_INST("DEFVAR GF@%input_prompt");
    ADD_INST("MOVE GF@%input_prompt string@?\\032");
    ADD_INST("DEFVAR GF@%tmp_op_1");
    ADD_INST("DEFVAR GF@%tmp_op_2");
    ADD_INST("DEFVAR GF@%tmp_op_3");
    ADD_INST("DEFVAR GF@%exp_result");
    ADD_INST("JUMP $$main");

    return true;
}

bool code_generator_start()
{
    if (!string_init(&code)) return false;
    if (!generate_file_header()) return false;
    if (!define_built_in_functions()) return false;

    return true;
}

void code_generator_clear()
{
    string_free(&code);
}

void code_generator_flush(FILE *destin_file)
{
    fputs(code.str, destin_file);
}

bool generate_main_start()
{
    ADD_INST("\n# Main");
    ADD_INST("LABEL $$main");
    ADD_INST("CREATEFRAME");
    ADD_INST("PUSHFRAME");

    return true;
}

bool generate_main_end(char *endval)
{
    ADD_INST("# End of main");
    ADD_INST("POPFRAME");
    ADD_INST("CLEARS");
    ADD_INST("EXIT");ADD_CODE(endval);

    return true;
}

bool generate_function_start(char *function_id)
{
    ADD_CODE("\n# Start of function "); ADD_CODE(function_id); ADD_CODE("\n");
    ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("\n");
    ADD_INST("PUSHFRAME");

    return true;
}

bool generate_function_end(char *function_id)
{
    ADD_CODE("# End of function "); ADD_CODE(function_id); ADD_CODE("\n");
    ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("%return\n");
    ADD_INST("POPFRAME");
    ADD_INST("RETURN");

    return true;
}
static bool generate_default_var_value(Data_Type type)
{
    switch (type)
    {
        case TYPE_INT:
            ADD_CODE("int@nil");
            break;

        case TYPE_FLOAT:
            ADD_CODE("float@nil");
            break;

        case TYPE_STRING:
            ADD_CODE("string@nil");
            break;

        case TYPE_BOOL:
            ADD_CODE("bool@nil");

        default:
            return false;
    }
    return true;
}

bool generate_function_retval(Data_Type type)
{
    ADD_INST("DEFVAR LF@%retval");
    ADD_CODE("MOVE LF@%retval ");
    if (!generate_default_var_value(type)) return false;
    ADD_CODE("\n");

    return true;
}

bool generate_function_call(char *function_id)
{
    ADD_CODE("CALL $"); ADD_CODE(function_id); ADD_CODE("\n");
    return true;
}

bool generate_function_retval_assign(char *l_val_id, Data_Type l_type, Data_Type ret_type)
{
    if (l_type == TYPE_INT && ret_type == TYPE_FLOAT)
    {
        ADD_INST("FLOAT2R2EINT TF@%retval TF@%retval");
    }
    else if (l_type == TYPE_FLOAT && ret_type == TYPE_INT)
    {
        ADD_INST("INT2FLOAT TF@%retval TF@%retval");
    }
    ADD_CODE("MOVE LF@"); ADD_CODE(l_val_id); ADD_CODE(" TF@%retval\n");

    return true;
}

bool generate_function_param_declare(char *param_id, int index)
{
    ADD_CODE("DEFVAR LF@"); ADD_CODE(param_id); ADD_CODE("\n");
    ADD_CODE("MOVE LF@"); ADD_CODE(param_id); ADD_CODE(" LF@%"); ADD_CODE_INT(index); ADD_CODE("\n");

    return true;
}

bool generate_function_before_pass_params()
{
    ADD_INST("CREATEFRAME");

    return true;
}

bool generate_function_convert_passed_param(Data_Type from, Data_Type to, int index)
{
    if (from == TYPE_FLOAT && to == TYPE_INT)
    {
        ADD_CODE("FLOAT2R2EINT TF@%"); ADD_CODE_INT(index); ADD_CODE(" TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");
    }
    else if (from == TYPE_INT && to == TYPE_FLOAT)
    {
        ADD_CODE("INT2FLOAT TF@%"); ADD_CODE_INT(index); ADD_CODE(" TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");
    }

    return true;
}

 static bool generate_term_value(tToken token)
{
    char term_str[MAX_DIG];
    unsigned char c;
    dynamic_string tmp_string;
    if (!string_init(&tmp_string)) return false;
    switch (token.Type)
    {
        case TT_INTEGER:
            sprintf(term_str, "%d", token.Data.integer);
            ADD_CODE("int@"); ADD_CODE(term_str);
            break;
        case TT_FLOAT:
            sprintf(term_str, "%g", token.Data.decimal);
            ADD_CODE("float@"); ADD_CODE(term_str);
            break;
        case TT_STRING:
            for (int i = 0; (c = (unsigned char) (token.Data.string->str)[i]) != '\0'; i++)
            {
                if (c == '#' || c == '\\' || c <= 32 || !isprint(c))
                {
                    string_add(&tmp_string, '\\');
                    sprintf(term_str, "%03d", c);
                    string_add_const(&tmp_string, term_str);
                }
                else
                {
                    string_add(&tmp_string, c);
                }
            }
            ADD_CODE("string@"); ADD_CODE(tmp_string.str);
            break;
        case TT_IDENTIFIER:
            ADD_CODE("LF@"); ADD_CODE(token.Data.string->str);
            break;
        default:
            string_free(&tmp_string);
            return false;
    }
    string_free(&tmp_string);

    return true;
}


bool generate_function_pass_param(tToken token, int index)
{
    ADD_CODE("DEFVAR TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");
    ADD_CODE("MOVE TF@%"); ADD_CODE_INT(index); ADD_CODE(" ");
    if (!generate_term_value(token)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_function_return(char *function_id)
{
    ADD_INST("MOVE LF@%retval GF@%exp_result");
    ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%return\n");

    return true;
}


bool generate_var_define(char *var_id)
{
    ADD_CODE("DEFVAR LF@"); ADD_CODE(var_id); ADD_CODE("\n");

    return true;
}


bool generate_var_default_value(char *var_id, Data_Type type)
{
    ADD_CODE("MOVE LF@"); ADD_CODE(var_id); ADD_CODE(" ");
    if (!generate_default_var_value(type)) return false;
    ADD_CODE("\n");

    return true;
}

bool generate_push(tToken token)
{
    ADD_CODE("PUSHS ");
    if (!generate_term_value(token)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_stack_operation(Prec_rules rule)
{
    switch (rule)
    {
        case E_PLUS_E:
            ADD_INST("ADDS");
            break;
        case E_MINUS_E:
            ADD_INST("SUBS");
            break;
        case E_MUL_E:
            ADD_INST("MULS");
            break;
        case E_DIV_E:
            ADD_INST("DIVS");
            break;

        case E_EQ_E:
            ADD_INST("EQS");
            break;

        case E_NEQ_E:
            ADD_INST("EQS");
            ADD_INST("NOTS");
            break;

        case E_LSEQ_E:
            ADD_INST("POPS GF@%tmp_op1");
            ADD_INST("POPS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op1");
            ADD_INST("LTS");
            ADD_INST("PUSHS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op1");
            ADD_INST("EQS");
            ADD_INST("ORS");
            break;

        case E_LSTN_E:
            ADD_INST("LTS");
            break;

        case E_MREQ_E:
            ADD_INST("POPS GF@%tmp_op1");
            ADD_INST("POPS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op1");
            ADD_INST("GTS");
            ADD_INST("PUSHS GF@%tmp_op2");
            ADD_INST("PUSHS GF@%tmp_op1");
            ADD_INST("EQS");
            ADD_INST("ORS");
            break;

        case E_MRTN_E:
            ADD_INST("GTS");
            break;

        default:
            break;
    }

    return true;
}


bool generate_concat_stack_strings()
{
    ADD_INST("POPS GF@%tmp_op3");
    ADD_INST("POPS GF@%tmp_op2");
    ADD_INST("CONCAT GF@%tmp_op1 GF@%tmp_op2 GF@%tmp_op3");
    ADD_INST("PUSHS GF@%tmp_op1");

    return true;
}


bool generate_save_expression_result(char *var_id, Data_Type ret_type, Data_Type l_type, char *frame)
{
    if (l_type == TYPE_INT && ret_type == TYPE_FLOAT)
    {
        ADD_INST("FLOAT2R2EINTS");
    }
    else if (l_type == TYPE_FLOAT && ret_type == TYPE_INT)
    {
        ADD_INST("INT2FLOATS");
    }

    ADD_CODE("POPS "); ADD_CODE(frame); ADD_CODE("@"); ADD_CODE(var_id); ADD_CODE("\n");

    return true;
}


bool generate_stack_op1_to_double()
{
    ADD_INST("INT2FLOATS");

    return true;
}


bool generate_stack_op1_to_integer()
{
    ADD_INST("FLOAT2R2EINTS");

    return true;
}


bool generate_stack_op2_to_double()
{
    ADD_INST("POPS GF@%tmp_op1");
    ADD_INST("INT2FLOATS");
    ADD_INST("PUSHS GF@%tmp_op1");

    return true;
}


bool generate_stack_op2_to_integer()
{
    ADD_INST("POPS GF@%tmp_op1");
    ADD_INST("FLOAT2R2EINTS");
    ADD_INST("PUSHS GF@%tmp_op1");

    return true;
}

static bool generate_label(char *function_id, int label_index, int label_depth)
{
    ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_depth);
    ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE("\n");

    return true;
}


bool generate_if_head()
{
    ADD_INST("\n# if then");

    return true;
}


bool generate_if_start(char *function_id, int label_index, int label_depth)
{
    ADD_CODE("JUMPIFEQ $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_depth);
    ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE(" GF@%exp_result bool@false\n");

    return true;
}


bool generate_if_else_part(char *function_id, int label_index, int label_depth)
{
    ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_depth);
    ADD_CODE("%"); ADD_CODE_INT(label_index + 1); ADD_CODE("\n");
    ADD_INST("# else");
    if (!generate_label(function_id, label_index, label_depth)) return false;

    return true;
}


bool generate_if_end(char *function_id, int label_index, int label_depth)
{
    ADD_INST("# end");
    if (!generate_label(function_id, label_index, label_depth)) return false;

    return true;
}


bool generate_while_head(char *function_id, int label_index, int label_depth)
{
    ADD_INST("\n# While Do");
    if (!generate_label(function_id, label_index, label_depth)) return false;

    return true;
}


bool generate_while_start(char *function_id, int label_index, int label_depth)
{
    ADD_CODE("JUMPIFEQ $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_depth);
    ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE(" GF@%exp_result bool@false"); ADD_CODE("\n");

    return true;
}


bool generate_while_end(char *function_id, int label_index, int label_depth)
{
    ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_depth);
    ADD_CODE("%"); ADD_CODE_INT(label_index - 1); ADD_CODE("\n");
    ADD_INST("# Loop");
    if (!generate_label(function_id, label_index, label_depth)) return false;

    return true;
}