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

		GET_TOKEN_AND_CHECK_RULE(param);

		GET_TOKEN_AND_CHECK_TYPE(TT_EOL);

		GET_TOKEN_AND_CHECK_RULE(command);

		CHECK_KEYWORD(KW_END);

		GET_TOKEN();
		return statement(data);
	}
}
