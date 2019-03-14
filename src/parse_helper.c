// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "parse_helper.h"
#include "sroc.h"
#include "string_helper.h"

/**
 * Get a section and add it to the context
 */
static bool add_section(struct parser_context *context)
{
        struct sroc_table *section = get_section(context);

        if (section == NULL) {
                return false;
        }

        printf("FOUND SECTION: %s\n", section->key);

        sroc_destroy_table(section);

        return true;
}

/**
 * Consume the currently focused line refrenced to by the context
 *
 * This will place the pos at the end of the line
 */
static void consume_line(struct parser_context *context)
{
        while (context->buffer[context->pos] != '\n') {
                increment_parser_context(context);
        }
}

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

int parse_line(struct parser_context *context, char ch)
{
        switch (char_to_token(ch)) {
        case OPEN_BRACKET:
                // Handle a new section
                if (!add_section(context)) {
                        return -1;
                }

                return 0;

                break;
        case ALPHA_CHAR:
                // Handle a new declaration
                if (!is_valid_declaration(context)) {
                        return -1;
                }

                return 0;

                break;
        case COMMENT_START:
                // Skip the line since it's a comment
                consume_line(context);

                return 0;

                break;
        default:
                return -1;
        }

        return -1;
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

/**
 * Update the parsing context after a single character iteration through the
 * parser buffer.
 *
 * When a new line is encountered reset the col_num and increment the line_num
 */
void increment_parser_context(struct parser_context *context)
{
        if (context->buffer[context->pos] == '\n') {
                // Handle a new line in the buffer
                ++context->line_num;
                context->col_num = 0;
        }

        ++context->pos;
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
