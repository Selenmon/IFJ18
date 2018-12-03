#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include <stdlib.h>

#include "dynamic_string.h"

FILE *SourceFile;

typedef enum {

	//Keywords.
	KW_DEF,
	KW_DO,
	KW_ELSE,
	KW_END,
	KW_IF,
	KW_NOT,
	KW_NIL,
	KW_THEN,
	KW_WHILE,

	//Built-in functions.
	KW_INPUTS,
	KW_INPUTI,
	KW_INPUTF,
	KW_PRINT,
	KW_LENGTH,
	KW_SUBSTR,
	KW_ORD,
	KW_CHR,

}  tKeyword;


typedef enum {
	TT_KEYWORD,
	TT_IDENTIFIER,
	TT_EOL, 
	TT_EOF,
	TT_EMPTY,


	TT_INTEGER, 
	TT_FLOAT, 
	TT_STRING, 

	TT_ERROR,

	TT_PLUS, 
	TT_MINUS, 
	TT_MUL, 
	TT_DIV, 

	TT_ASSIGN, // =
	TT_LESS_THAN, // <
	TT_MORE_THAN, // >
	TT_LESS_OR_EQUAL, // <=
	TT_MORE_OR_EQUAL, // >=
	TT_IS_EQUAL, // ==
	TT_NOT_EQUAL, // !=


	TT_COMMA, 
	TT_LEFT_BRACKET, 
	TT_RIGHT_BRACKET, 

} tTokenType;

typedef enum {
	
	ST_START, 
	ST_LINE_COMMENTARY, 
	ST_ASSIGN, 
	ST_BLOCK_COMMENTARY,
	
	ST_ID_KEYWORD,
	
	ST_NUMBER, 
	ST_NUMBER_DECIMAL,
	ST_NUMBER_POINT,
	ST_NUMBER_EXPONENT,
	ST_NUMBER_EXPONENT_SIGN,
	ST_NUMBER_EXPONENT_END,

	ST_STRING_HEXA, 
	ST_STRING,  
	ST_STRING_ESCAPE, 
	
	ST_LESS_THAN, 
	ST_MORE_THAN, 
	ST_NOT_EQUAL, 

	ST_EOL, 

} tState;

//Data of token.
typedef union {

	dynamic_string *string;
	int integer;
	double decimal;
	tKeyword Keyword;

} tTokenData;

//Structure of token.
typedef struct {
	
	tTokenType Type;
	tTokenData Data;

} tToken;

int freeDynamicString(int errorcode, dynamic_string *string);
int getIdentifier(dynamic_string *string, tToken *Token);
int getInteger(dynamic_string *string, tToken *Token);
int getDecimal(dynamic_string *string, tToken *Token);
int getToken(tToken *Token);
#endif //_SCANNER_H
