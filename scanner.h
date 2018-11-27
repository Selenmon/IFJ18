#include <stdio.h>
#include <stdlib.h>

#include "dynamic_string.h"

FILE *SourceFile;

typedef enum {

	//klicova slova
	KW_DEF,
	KW_DO,
	KW_ELSE,
	KW_END,
	KW_IF,
	KW_NOT,
	KW_NIL,
	KW_THEN,
	KW_WHILE,

	//vestavene funkce
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
	TT_EOL, // konec radku
	TT_EMPTY,

	TT_INTEGER, //cele cislo
	TT_DOUBLE, //desetinne 
	TT_STRING, //retezec

	TT_ERROR, //nepovoleny lexem

	//operatory
	TT_PLUS, // +
	TT_MINUS, // -
	TT_MUL, // *
	TT_DIV, // /

	TT_ASSIGN, // =
	TT_LESS_THAN, // <
	TT_MORE_THAN, // >
	TT_LESS_OR_EQUAL, // <=
	TT_MORE_OR_EQUAL, // >=
	TT_IS_EQUAL, // ==
	TT_NOT_EQUAL, // !=

	//oddelovace
	TT_DOT, // . ocekavam desetinne cislo
	TT_COMMA, // , ocekavam argumenty
	TT_LEFT_BRACKET, // (
	TT_RIGHT_BRACKET, // )

} tTokenType;

typedef enum {
	ST_START, // start
	ST_LINE_COMMENTARY, // # , =begin mezera/tabulator =end
	ST_ASSIGN, // =
	ST_BLOCK_COMMENTARY_START, // =begin
	ST_BLOCK_COMMENTARY, // =end

	ST_IDENTIFIER, //zacina malym pismenem nebo _, obsahuje mala velka pismena, muze koncit !?
	ST_KEYWORD,

	ST_NUMBER, //cislice, nesmi zacinat 0
	ST_NUMBER_DECIMAL, // . cislice bude desetinna
	ST_NUMBER_EXPONENT, //  e nebo E cislice ma exponent, muze mit i + a -

	ST_STRING_START, // "
	ST_STRING, // dalsi ", znaky s hodnotou ASCII > 31 rovnou, 
	ST_STRING_ESCAPE, // escape sekvence \", \n, \s, \t, \xhh
	
	ST_LESS_THAN, // zacina <
	ST_MORE_THAN, // zacina >

	ST_EOL, // end of line

	//TODO?
} tState;

//co to bude za token
typedef union {

	dynamic_string *string;
	int integer;
	double decimal;
	tKeyword Keyword;

} tTokenData;

//struktura tokenu pro parser
typedef struct token {
	
	tTokenType Type;
	tTokenData Data;

} tToken;

extern tToken TokenStructure;


int getToken(TokenStructure *Token);
