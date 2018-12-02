#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"
#include "dynamic_string.h"


FILE *SourceFile;

dynamic_string *Dynamic_string;


//Function to free dynamic string. Returns error code.
int freeDynamicString(int errorcode, dynamic_string *string) {
	
	string_free(string);

	return errorcode;
}

//Function to get identifier or keyword type of token.
int getIdentifier(dynamic_string *string, tToken *Token) {
	if (!string_cmp(string, "def"))
		Token.Data->Keyword = KW_DEF;
	else if (!string_cmp(string, "do"))
		Token.Data->Keyword = KW_DO;
	else if (!string_cmp(string, "else"))
		Token.Data->Keyword = KW_ELSE;
	else if (!string_cmp(string, "end"))
		Token.Data->Keyword = KW_END;
	else if (!string_cmp(string, "if"))
		Token.Data->Keyword = KW_IF;
	else if (!string_cmp(string, "not"))
		Token.Data->Keyword = KW_NOT;
	else if (!string_cmp(string, "nil"))
		Token.Data->Keyword = KW_NIL;
	else if (!string_cmp(string, "then"))
		Token.Data->Keyword = KW_THEN;
	else if (!string_cmp(string, "while"))
		Token.Data->Keyword = KW_WHILE;
	else if (!string_cmp(string, "inputs"))
		Token.Data->Keyword = KW_INPUTS;
	else if (!string_cmp(string, "inputi"))
		Token.Data->Keyword = KW_INPUTI;
	else if (!string_cmp(string, "inputf"))
		Token.Data->Keyword = KW_INPUTF;
	else if (!string_cmp(string, "print"))
		Token.Data->Keyword = KW_PRINT;
	else if (!string_cmp(string, "length"))
		Token.Data->Keyword = KW_LENGTH;
	else if (!string_cmp(string, "substr"))
		Token.Data->Keyword = KW_SUBSTR;
	else if (!string_cmp(string, "ord"))
		Token.Data->Keyword = KW_ORD;
	else if (!string_cmp(string, "chr"))
		Token.Data->Keyword = KW_CHR;
	else
		Token->Type = TT_IDENTIFIER;

	if (Token->Type != TT_IDENTIFIER) {

		Token->Type = TT_KEYWORD;
		return freeDynamicString(SCANNER_TOKEN_OK, string);
	}

	if (!string_cp = (string, Token->Data.string)) {

		return freeDynamicString(ERROR_INTERNAL, string);
	}
	
	return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get integer type of token.
int getInteger(dynamic_string *string, tToken *Token) {

	char *ptr;
	int integer = (int)strtol(string->str, &ptr, 10);
	
	if (*ptr) 
		return freeDynamicString(ERROR_INTERNAL, string);

	Token->Data.integer = integer;
	Token->Type = TT_INTEGER;

	return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get float type of token.
int getDecimal(dynamic_string *string, tToken *Token) {
	
	char *ptr;
	int decimal = (int)strtod(string->str, &ptr, 10);

	if (*ptr)
		return freeDynamicString(ERROR_INTERNAL, string);

	Token->Data.integer = decimal;
	Token->Type = TT_INTEGER;

	return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get appropriate type of token. Main function.
int getToken(tToken *Token) {

	if ((SourceFile == NULL) | (Dynamic_string == NULL))
		return ERROR_INTERNAL;

	Token->Data.string = Dynamic_string;

	tState State = ST_START;
	Token->Type = TT_EMPTY;

	dynamic_string string;
	dynamic_string *str = &string;
	char c;

	while (c = (getc(SourceFile))) {

		switch (State) {

		case (ST_START):
			if (c == '\n') {
				State = ST_EOL;
			}
			else if (c == '#') {
				State = ST_LINE_COMMENTARY;
			}
			else if (c == '=') {
				State = ST_ASSIGN;
			}
			else if (c == '<') {
				State = ST_LESS_THAN;
			}
			else if (c == '>') {
				State = ST_MORE_THAN;
			}
			else if (c == '!') {
				State = ST_NOT_EQUAL;
			}
			else if (c == '+') {
				Token->Type = TT_PLUS;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == '-') {
				Token->Type = TT_MINUS;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == '*') {
				Token->Type = TT_MUL;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == '/') {
				Token->Type = TT_DIV;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == '(') {
				Token->Type = TT_LEFT_BRACKET;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == ')') {
				Token->Type = TT_RIGHT_BRACKET;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == ',') {
				Token->Type = TT_COMMA;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else if (c == '.') {
				Token->Type = TT_DOT;
				return freeDynamicString(SCANNER_TOKEN_OK, string);
			}
			else
				Token->Type = TT_ERROR;
				return freeDynamicString(SCANNER_ERROR_LEX, string);

			break;
		
		case (ST_LINE_COMMENTARY):
			if (c == '\n') {
				State = ST_START;
				ungetc(c, SourceFile); //vrati zpatky pozici predchoziho getc
			}

			break;
		case (ST_ASSIGN):
			if (c == '=') {
				Token->Type = TT_IS_EQUAL;
			}
			else {
				ungetc(c, SourceFile);
				Token->Type = TT_ASSIGN;
			}
			return freeDynamicString(SCANNER_TOKEN_OK, string);
		case (ST_LESS_THAN):
			if (c == '=') {
				Token->Type = TT_LESS_OR_EQUAL;
			}
			else {
				ungetc(c, SourceFile);
				Token->Type = TT_LESS_THAN;
			}
			return freeDynamicString(SCANNER_TOKEN_OK, string);
		case (ST_MORE_THAN):
			if (c == '=') {
				Token->Type = TT_MORE_OR_EQUAL;
			}
			else {
				ungetc(c, SourceFile);
				Token->Type = TT_MORE_THAN;
			}
			return freeDynamicString(SCANNER_TOKEN_OK, string);
		case (ST_NOT_EQUAL):
			if (c == '=') {
				Token->Type = TT_NOT_EQUAL;
			}
			else {
				Token->Type = TT_ERROR;
				return freeDynamicString(SCANNER_ERROR_LEX, string);
			}
			
		}

	}


}
