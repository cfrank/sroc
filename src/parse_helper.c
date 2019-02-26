// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "parse_helper.h"
#include "sroc.h"

enum token_type char_to_token(char input)
{
        switch (input) {
        case ']':
                return CLOSE_BRACKET;
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

int64_t find_first_nonspace(const char *string)
{
        char ch;
        int64_t index = 0;

        while ((ch = string[index]) != '\0' && isspace(ch)) {
                ++index;
        }

        if (ch == '\0') {
                return -1;
        }

        return index;
}

int64_t find_last_nonspace(const char *string)
{
        for (int i = (int)strlen(string) - 1; i >= 0; --i) {
                if (!isspace(string[i])) {
                        return i;
                }
        }

        return -1;
}

