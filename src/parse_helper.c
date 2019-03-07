// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "parse_helper.h"
#include "sroc.h"
#include "string_helper.h"

enum token_type char_to_token(char input)
{
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
                break;
        }

        if (isalpha(input)) {
                return ALPHA_CHAR;
        }

        if (isdigit(input)) {
                return NUMERIC_CHAR;
        }

        return UNKNOWN;
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

bool is_valid_declaration(struct parser_context *context)
{
        char *name;
        int64_t name_len = string_get_delimiter(
                context->buffer + context->pos, '=', &name);

        if (name_len < 0) {
                return false;
        }

        printf("DECL Name: %s\n", name);

        free(name);

        return true;
}

struct sroc_table *get_section(struct parser_context *context)
{
        if (char_to_token(context->buffer[context->pos]) != OPEN_BRACKET) {
                return NULL;
        }

        char *key;

        int64_t key_len = string_get_delimiter(
                context->buffer + (context->pos + 1), ']', &key);

        if (key_len < 0) {
                return NULL;
        }

        struct sroc_table *section = sroc_create_table(key);

        if (section == NULL) {
                free(key);

                return NULL;
        }

        return section;
}

bool is_valid_section(struct parser_context *context)
{
        if (char_to_token(context->buffer[context->pos]) != OPEN_BRACKET) {
                return -1;
        }

        char *name;

        int64_t name_len = string_get_delimiter(
                context->buffer + (context->pos + 1), ']', &name);

        if (name_len < 0) {
                return false;
        }

        printf("FOUND SECTION NAME: %s\n", name);

        free(name);

        return true;
}

void destroy_parser_context(struct parser_context *context)
{
        free(context);
}
