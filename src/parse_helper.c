// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "parse_helper.h"
#include "sroc.h"

enum token_type char_to_token(char input)
{
        if (isalpha(input)) {
                return ALPHA_CHAR;
        }

        if (isdigit(input)) {
                return NUMERIC_CHAR;
        }

        switch (input) {
        case ']':
                return CLOSE_BRACKET;
        case ';':
        case '#':
                return COMMENT_START;
        case ',':
                return COMMA;
        case '=':
                return EQUAL;
        case '\\':
                return ESCAPE;
        case '-':
                return NEGATIVE;
        case '[':
                return OPEN_BRACKET;
        case '.':
                return PERIOD;
        case '"':
                return QUOTE;
        default:
                return UNKNOWN;
        }
}

struct parser_context *init_parser(void)
{
        struct parser_context *context = malloc(sizeof(struct parser_context));

        if (context == NULL) {
                errno = ENOMEM;

                return NULL;
        }

        context->buffer = NULL;
        context->pos = 0;
        context->line_num = 0;
        context->col_num = 0;
        context->current_value = NULL;
        context->current_table = NULL;

        return context;
}

void destroy_parser_context(struct parser_context *context)
{
        free(context);
}
