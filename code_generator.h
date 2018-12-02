
#ifndef IFJ18_CODE_GENERATOR_H
#define IFJ18_CODE_GENERATOR_H

#include <stdio.h>
#include <stdbool.h>

#include "bstsymtable.h"
#include "scanner.h"
#include "expression.h"
#include "code_generator.h"


/**
 * @brief Inicialization of code generator, defines built-in functions
 *
 * @return True if successful, false otherwise
 */
bool code_generator_start();

/**
 * @brief Function clears allocated resources
 */
void code_generator_clear();

/**
 * @brief Function flushes generated code to destination file
 *
 * @param destination_file Pointer to destination file
 */
void code_generator_flush(FILE *destination_file);

/**
 * @brief Function generates start for main program code
 *
 * @return True if successful, false otherwise
 */
bool generate_main_scope_start();

/**
 * @brief Function generates end for main main program code
 *
 * @return True if successful, false otherwise
 */
bool generate_main_scope_end();

/**
 * @brief Function generates code for start of function
 *
 * @param function_id Function identifier
 * @return True if successful, false otherwise
 */
bool generate_function_start(char *function_id);

/**
 * @brief Function generates code for end of function
 *
 * @param function_id Function identifier
 * @return True if successful, false otherwise
 */
bool generate_function_end(char *function_id);

/**
 * @brief Funtion generates code for definition of function return value
 *
 * @param type Data type of function return value
 * @return True if successful, false otherwise
 */
bool generate_function_retval(Data_type type);

/**
 * @brief Function generates code for function call (jump to the function)
 *
 * @param function_id Function identifier
 * @return True if successful, false otherwise
 */
bool generate_function_call(char *function_id);

/**
 * @brief Function generates code for return value of assignment function of L-variable.
 *
 * @param l_val_id Identifier of L-variable
 * @param l_type L-variable data type
 * @param ret_type Data type of function return value
 * @return True if successful, false otherwise
 */
bool generate_function_retval_assign(char *l_val_id, Data_type l_type, Data_type ret_type);

/**
 * @brief Function generates code for local variable for function parameter.
 *
 * @param param_id Parameter identifier
 * @param index Parameter index (position)
 * @return True if successful, false otherwise
 */
bool generate_function_param_declare(char *param_id, int index);

/**
 * @brief Function generates code for preparation for pass parameters to function
 *
 * @return True if successful, false otherwise
 */
bool generate_function_before_pass_params();

/**
 * @brief Function generates code for passing parameter to function
 *
 * @param token Token with passed parameter value
 * @param index Parameter index (position)
 * @return True if successful, false otherwise
 */
bool generate_function_pass_param(Token token, int index);

/**
 * @brief Function generates code for conversion of passed parameter data type
 *
 * @param from Data type to convert from
 * @param to Data type to conver to
 * @param index Parameter index (position)
 * @return True if successful, false otherwise
 */
bool generate_function_convert_passed_param(Data_type from, Data_type to, int index);

/**
 * @brief Function generates code for return statement in function
 *
 * @param function_id Function identifier
 * @return True if successful, false otherwise
 */
bool generate_function_return(char *function_id);

/**
 * @brief Function generates code for variable definition
 *
 * @param var_id Variable identifier
 * @return True if successful, false otherwise
 */
bool generate_var_define(char *var_id);

/**
 * @brief Function generates code for assignment default value to variable.
 *
 * @param var_id Variable identifier
 * @param type Data type of variable
 * @return True if successful, false otherwise
 */
bool generate_var_default_value(char *var_id, Data_type type);

/**
 * @brief Function code for generates input statement
 *
 * @param var_id Identifier of variable read value
 * @param type Data type of read value
 * @return True if successful, false otherwise
 */
bool generate_input(char *var_id, Data_type type);

/**
 * @brief Function generates code for print of expression result
 *
 * @return True if successful, false otherwise
 */
bool generate_print();

/**
 * @brief Function generates code for push value to data-stack
 *
 * @param token Token with value to be pushed
 * @return True if successful, false otherwise
 */
bool generate_push(Token token);

/**
 * @brief Function generates code for operation with top data-stack items
 *
 * @param rule Expression rule
 * @return True if successful, false otherwise
 */
bool generate_stack_operation(Prec_rules_enum rule);

/**
 * @brief Function generates code for concatenation of top data-stack items
 *
 * @return True if successful, false otherwise
 */
bool generate_concat_stack_strings();

/**
 * @brief Function generates code for save expression result from data-stack
 *
 * @param var_id Variable identifier for expression result
 * @param ret_type Data type of expression in data stack
 * @param l_type Data type of variable for save result
 * @param frame Variable frame of given variable
 * @return True if successful, false otherwise
 */
bool generate_save_expression_result(char *var_id, Data_type ret_type, Data_type l_type, char *frame);

/**
 * @brief Function generates code for conversion of data-stack top-item to double
 *
 * @return True if successful, false otherwise
 */
bool generate_stack_op1_to_double();

/**
 * @brief Function generates code for conversion of data-stack top item to integer
 *
 * @return True if successful, false otherwise
 */
bool generate_stack_op1_to_integer();

/**
 * @brief Function generates code for conversion of data-stack top-1 item to double
 *
 * @return True if successful, false otherwise
 */
bool generate_stack_op2_to_double();

/**
 * @brief Function generates code for conversion of data-stack top-1 item to integer
 *
 * @return True if successful, false otherwise
 */
bool generate_stack_op2_to_integer();

/**
 * @brief Function generates code for If-Then head (before processing If-Then expression)
 *
 * @return True if successful, false otherwise
 */
bool generate_if_head();

/**
 * @brief Function generates code for If-Then start (after processing If-Then expression)
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_if_start(char *function_id, int label_index, int label_depth);

/**
 * @brief Function generates code for If-Then Else part (after processing If-Then statement)
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_if_else_part(char *function_id, int label_index, int label_depth);

/**
 * @brief Function generates code for If-Then end (after processing If-Then Else statement)
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_if_end(char *function_id, int label_index, int label_depth);

/**
 * @brief Function generates code for Do-While head (before processing expression)
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_while_head(char *function_id, int label_index, int label_depth);

/**
 * @brief Function generates code for Do-While head (after processing expression)
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_while_start(char *function_id, int label_index, int label_depth);

/**
 * @brief Function generates code for Do-While head (after processing statement).
 *
 * @param function_id Identifier of function in which is this statement
 * @param label_index Index of label
 * @param label_depth Depth of label
 * @return True if successful, false otherwise
 */
bool generate_while_end(char *function_id, int label_index, int label_depth);



#endif //IFJ18_CODE_GENERATOR_H
