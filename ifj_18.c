#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "code_generator.h"

int main()
{
    FILE* source_file;

    source_file = stdin;
    set_source_file(source_file);

    int error_code;
    if ((error_code = analyse()))
    {
        code_generator_clear();
        return error_code;
    }

    code_generator_flush(stdout);

    return EXIT_SUCCESS;
}

