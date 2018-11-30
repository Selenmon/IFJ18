#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"
#include "dynamic_string.h"


FILE *SourceFile;

dynamic_string *Dynamic_string


//Function to free dynamic string
int freeDynamicString(int exit_code, dynamic_string *string) {
	
	string_free(string);

	return exit_code;
}


int getIdentifier(dynamic_string *string, tToken *Token) {
	//TODO
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
		//TODO
	}

	//TODO
}

int getInteger(dynamic_string *string, tToken *Token) {
	//TODO
}

int getDecimal(dynamic_string *string, tToken *Token) {
	//TODO
}

//hlavni funkce
int getToken(TokenStructure *Token) {

	tState State = ST_START;
	Token->Type = TT_EMPTY;

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
			else if (c == '+') {
				Token->Type = TT_PLUS;
				//TODO
			}
			else if (c == '-') {
				Token->Type = TT_MINUS;
				//TODO
			}
			else if (c == '*') {
				Token->Type = TT_MUL;
				//TODO
			}
			else if (c == '/') {
				Token->Type = TT_DIV;
				//TODO
			}
			else if (c == '(') {
				Token->Type = TT_LEFT_BRACKET;
				//TODO
			}
			else if (c == ')') {
				Token->Type = TT_RIGHT_BRACKET;
				//TODO
			}
			else if (c == ',') {
				Token->Type = TT_COMMA;
				//TODO
			}
			//else TODO
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
		//TODO

		}

	}


}


/*
else if (c == '<') {
Token->Type = TT_LESS_OR_EQUAL;
}
else if (c == {
Token->Type = TT_MORE_OR_EQUAL;
}

*/
