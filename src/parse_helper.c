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

/**
 * Finds the first nonspace character in a string. If nothing is found -1 is
 * returned.
 *
 * If a nonspace character is found the index of that character is returned
 */
int64_t find_first_nonspace(const char *string)
{
        char ch;
        int64_t index = 0;

        while ((ch = string[index]) != '\0' && isspace(ch)) {
                ++index;
        }

        if (ch == '\0') {
                // Reached the end without finding a nonspace char
                return -1;
        }

        return index;
}

/**
 * Finds the last nonspace character in a string. If nothing is found -1 is
 * returned.
 *
 * If a nonspace character is found the index of that character is returned
 */
int64_t find_last_nonspace(const char *string)
{
        int64_t index = strlen(string) - 1;

        while (index >= 0 && isspace(string[index])) {
                --index;
        }

        // If nothing is found will have reached -1
        return index;
}

/**
 * Strips the whitespace surrounding a string and places that new string into
 * the destination with an added null terminator.
 *
 * The strlen of the new string is returned.
 */
int64_t strip_surrounding_space(const char *string, char **dest)
{
        int64_t start_index = find_first_nonspace(string);
        int64_t end_index = find_last_nonspace(string);

        if (start_index < 0 || end_index < 0) {
                return -1;
        }

        size_t stripped_size = (end_index - start_index) + 1;

        *dest = malloc(stripped_size + 1);

        if (*dest == NULL) {
                return -1;
        }

        memcpy(*dest, string + start_index, stripped_size);

        (*dest)[stripped_size] = '\0';

        return stripped_size;
}

