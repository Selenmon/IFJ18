#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"
#include "dynamic_string.h"


FILE *SourceFile;

dynamic_string *Dynamic_string


int freeDynamicString(int exit_code, dynamic_string *string) {

    string_free(string);

    return exit_code;
}

int getIdentifier(dynamic_string *string, tToken *Token) {
    //TODO
}

int getInteger(dynamic_string *string, tToken *Token) {
    //TODO
}

int getDecimal(dynamic_string *string, tToken *Token) {
    ) {
        //TODO
    }

//hlavni funkce
    int getToken(tToken *Token) {

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