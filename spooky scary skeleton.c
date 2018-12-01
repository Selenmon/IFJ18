#include "dynamic_string.h"
#include "error.h"
#include "symtable.h"
#include "analysis.h"
#include "code_generator.h"

static int program(PData* data);
static int parameters(PData* data);
static int var(PData* data);
static int command(PData* data);

#define IS_VALUE(token)												
	(token).type == TOKEN_TYPE_DOUBLE_NUMBER || (token).type == TOKEN_TYPE_INT_NUMBER || (token).type == TOKEN_TYPE_STRING	|| (token).type == TOKEN_TYPE_IDENTIFIER

static int program(PData* data)
{
	int result;

	// PROGRAM -> DEF ID ( PARAMETERS ) EOL COMMAND END
	if (data->token.type == TT_KEYWORD && data->token.attribute.keyword == KW_DEF)
	{
		data->declaring = true;
		data->non_declared = true;
		data->index = 0;

		GET_TOKEN_AND_CHECK_TYPE(TT_IDENTIFIER);
		GET_TOKEN_AND_CHECK_TYPE(TT_LEFT_BRACKET);

		//SELENMON ADD NAME OF FCTION TO SYMTABLE

		GET_TOKEN_AND_CHECK_RULE(parameters);
		GET_TOKEN_AND_CHECK_TYPE(TT_RIGHT_BRACKET);
		GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

		//type

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);
		GET_TOKEN_AND_CHECK_RULE(command);
		CHECK_KEYWORD(KW_END);

		data->current_id->declared = true;

		// NEXT
		GET_TOKEN();
		return prog(data);
	}
	return SYNTAX_ERR;
}

static int parameters(PData* data)
{
	int result;
	data->parameter_index = 0;

	// PARAMETERS -> VAR PARAMETERS_NO
	if(CHECK_RULE(var) == var)
	{
		CHECK_RULE(parameters_no);
	}
	return SYNTAX_OK;
}

static int parameters_no(PData* data)
{
	int result;

	// PARAMETERS_NO -> , VAR
	if (data->token.type == TT_COMMA)
	{
		data->parameter_index++;

		CHECK_RULE(var);

		GET_TOKEN();
		return param_n(data)
	}
	return SYNTAX_OK;
}

static int var(PData* data)
{
	// var -> INT
	if (data->token.type == TT_INTEGER)
	{
		//check somethingorother
	}
	else if (data->token.type == TT_DOUBLE)
	{
		//check somethingorother
	}
	else if (data->token.type == TT_STRING)
	{
		//check somethingorother
	}
}

static int command(PData* data)
{
	int result;

	// COMMAND -> ID = EXPR EOL /  ID = FUNCTION EOL
	if(data->token.type == TT_IDENTIFIER)
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
	// COMMAND -> IF EXPR THEN EOL COMMAND END
	else if(data->token.type == TT_KEYWORD && data->token.data.keyword == KW_IF)
	{
		GET_TOKEN_AND_CHECK_RULE(expr);

		CHECK_KEYWORD(KW_THEN);

		GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

		GET_TOKEN_AND_CHECK_RULE(command);

		CHECK_KEYWORD(KW_END);
		
		GET_TOKEN();
		return statement(data);
	}
	// COMMAND -> WHILE EXPR DO EOL COMMAND END
	else if(data->token.type == TT_KEYWORD && data->token.data.keyword == KW_WHILE)
	{
		GET_TOKEN_AND_CHECK_RULE(expr);

		CHECK_KEYWORD(KW_DO);

		GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

		GET_TOKEN_AND_CHECK_RULE(command);

		CHECK_KEYWORD(KW_END);
		
		GET_TOKEN();
		return statement(data);	
	}
	// COMMAND -> DEF ID (param) EOL COMMAND END
	else if(data->token.type == TT_KEYWORD && dta->token.data.keyword == KW_DEF)
	{
		GET_TOKEN_AND_CHECK_TYPE(TT_IDENTIFIER);

		GET_TOKEN_AND_CHECK_RULE(parameters);

		GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

		GET_TOKEN_AND_CHECK_RULE(command);

		CHECK_KEYWORD(KW_END);

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

