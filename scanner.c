#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "error.h"
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
        Token->Data.Keyword = KW_DEF;
    else if (!string_cmp(string, "do"))
        Token->Data.Keyword = KW_DO;
    else if (!string_cmp(string, "else"))
        Token->Data.Keyword = KW_ELSE;
    else if (!string_cmp(string, "end"))
        Token->Data.Keyword = KW_END;
    else if (!string_cmp(string, "if"))
        Token->Data.Keyword = KW_IF;
    else if (!string_cmp(string, "not"))
        Token->Data.Keyword = KW_NOT;
    else if (!string_cmp(string, "nil"))
        Token->Data.Keyword = KW_NIL;
    else if (!string_cmp(string, "then"))
        Token->Data.Keyword = KW_THEN;
    else if (!string_cmp(string, "while"))
        Token->Data.Keyword = KW_WHILE;
    else if (!string_cmp(string, "inputs"))
        Token->Data.Keyword = KW_INPUTS;
    else if (!string_cmp(string, "inputi"))
        Token->Data.Keyword = KW_INPUTI;
    else if (!string_cmp(string, "inputf"))
        Token->Data.Keyword = KW_INPUTF;
    else if (!string_cmp(string, "print"))
        Token->Data.Keyword = KW_PRINT;
    else if (!string_cmp(string, "length"))
        Token->Data.Keyword = KW_LENGTH;
    else if (!string_cmp(string, "substr"))
        Token->Data.Keyword = KW_SUBSTR;
    else if (!string_cmp(string, "ord"))
        Token->Data.Keyword = KW_ORD;
    else if (!string_cmp(string, "chr"))
        Token->Data.Keyword = KW_CHR;
    else
        Token->Type = TT_IDENTIFIER;

    if (Token->Type != TT_IDENTIFIER) {

        Token->Type = TT_KEYWORD;
        return freeDynamicString(SCANNER_TOKEN_OK, string);
    }

    if (!string_cp(string, Token->Data.string)) {

        return freeDynamicString(ERROR_INTERNAL, string);
    }

    return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get integer type of token.
int getInteger(dynamic_string *string, tToken *Token) {

    char *ptr;
    int integer = strtol(string->str, &ptr);

    if (*ptr)
        return freeDynamicString(ERROR_INTERNAL, string);

    Token->Data.integer = integer;
    Token->Type = TT_INTEGER;

    return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get float type of token.
int getDecimal(dynamic_string *string, tToken *Token) {

    char *ptr;
    double decimal = strtod(string->str, &ptr);

    if (*ptr)
        return freeDynamicString(ERROR_INTERNAL, string);

    Token->Data.integer = decimal;
    Token->Type = TT_FLOAT;

    return freeDynamicString(SCANNER_TOKEN_OK, string);
}

//Function to get appropriate type of token.
int getToken(tToken *Token) {

    if ((SourceFile == NULL) || (Dynamic_string == NULL))
        return ERROR_INTERNAL;

    Token->Data.string = Dynamic_string;

    tState State = ST_START;
    Token->Type = TT_EMPTY;

    dynamic_string *string;

    char c;
    int counter = 0;
    char ascii[2];

    //Reading source file.
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
                else if ((islower(c)) || c == '_') {
                    State = ST_ID_KEYWORD;
                }
                else if (isdigit(c) && c != '0') {
                    State = ST_NUMBER;
                }
                else if (c == '"') {
                    State = ST_STRING;
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
                else if (c == EOF) {
                    Token->Type = TT_EOF;
                    return freeDynamicString(SCANNER_TOKEN_OK, string);
                }
                else
                    return freeDynamicString(SCANNER_ERROR_LEX, string);

                break;

            case (ST_LINE_COMMENTARY):
                if (c == '\n') {
                    State = ST_START;
                    ungetc(c, SourceFile);
                }

                break;

            case (ST_ASSIGN):
                if (c == 'b') {
                    c = (getc(SourceFile));
                    if (c == 'e') {
                        c = (getc(SourceFile));
                        if (c == 'g') {
                            c = (getc(SourceFile));
                            if (c == 'i') {
                                c = (getc(SourceFile));
                                if (c == 'n') {
                                    State = ST_BLOCK_COMMENTARY;
                                }
                            }
                        }
                    }
                }
                else if (c == '=') {
                    Token->Type = TT_IS_EQUAL;
                }
                else if (c == ' ') {
                    Token->Type = TT_ASSIGN;
                }
                return freeDynamicString(SCANNER_TOKEN_OK, string);

            case (ST_BLOCK_COMMENTARY):
                if (c == '=') {
                    c = (getc(SourceFile));
                    if (c == 'e') {
                        c = (getc(SourceFile));
                        if (c == 'n') {
                            c = (getc(SourceFile));
                            if (c == 'd') {
                                State = ST_START;
                            }
                        }
                    }
                }
                break;
            case (ST_ID_KEYWORD):
                if (isalnum(c) || c == '_' || c == '?' || c == '!') {
                    ungetc(c, SourceFile);
                    getIdentifier(string, Token);
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;

            case (ST_NUMBER):
                if (isdigit(c)) {
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else if (c == '.') {
                    State = ST_NUMBER_POINT;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else if (toupper(c) == 'E') {
                    State = ST_NUMBER_EXPONENT;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {
                    ungetc(c, SourceFile);
                    getInteger(string, Token);
                }
                break;

            case (ST_NUMBER_POINT):
                if (isdigit(c)) {
                    State = ST_NUMBER_DECIMAL;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;
            case (ST_NUMBER_DECIMAL):
                if (isdigit(c)) {
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else if (toupper(c) == 'E') {
                    State = ST_NUMBER_EXPONENT;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {
                    ungetc(c, SourceFile);
                    getDecimal(string, Token);
                }
                break;

            case (ST_NUMBER_EXPONENT):
                if (isdigit(c)) {
                    State = ST_NUMBER_EXPONENT_END;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else if (c == '-' || c == '+') {
                    State = ST_NUMBER_EXPONENT_SIGN;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;
            case (ST_NUMBER_EXPONENT_SIGN):
                if (isdigit(c)) {
                    State = ST_NUMBER_EXPONENT_END;
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;

            case (ST_NUMBER_EXPONENT_END):
                if (isdigit(c)) {

                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                }
                else {
                    ungetc(c, SourceFile);
                    getDecimal(string, Token);
                }
                break;

            case (ST_STRING):
                if (c == '"') {
                    if (!string_cp(string, Token->Data.string)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    ;

                    return freeDynamicString(SCANNER_TOKEN_OK, string);
                }
                else if (c == '\\') {
                    State = ST_STRING_ESCAPE;
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;

            case (ST_STRING_ESCAPE):
                if (c == 'n') {
                    c = '\n';
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    State = ST_STRING;
                }
                else if (c == '"')
                {
                    c = '"';
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    State = ST_STRING;
                }
                else if (c == 't')
                {
                    c = '\t';
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    State = ST_STRING;
                }
                else if (c == '\\')
                {
                    c = '\\';
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    State = ST_STRING;
                }
                else if (c == 's')
                {
                    c = ' ';
                    if (!string_add(string, c)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }
                    State = ST_STRING;
                }
                else if (c == 'x')
                {
                    State = ST_STRING_HEXA;
                }
                else {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;

            case (ST_STRING_HEXA):
                if (counter < 2 && isxdigit(c))
                {
                    ascii[counter] = c;
                    counter++;
                }
                else if (counter == 0) {

                    return freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                else
                {
                    long hexa = strtol(ascii, NULL, 16);

                    if (!string_add(string, hexa)) {
                        return freeDynamicString(ERROR_INTERNAL, string);
                    }

                    ungetc(c, SourceFile);
                    counter = 0;

                    State = ST_STRING;
                }
                break;


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
                    return freeDynamicString(SCANNER_TOKEN_OK, string);
                }
                else {
                    freeDynamicString(SCANNER_ERROR_LEX, string);
                }
                break;
            case (ST_EOL):
                if (isspace(c)) {
                    break;
                }

                ungetc(c, SourceFile);
                Token->Type = TT_EOL;
                return freeDynamicString(SCANNER_TOKEN_OK, string);
        }
    }
}