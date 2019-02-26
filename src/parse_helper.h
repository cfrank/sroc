// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include "sroc.h"

enum token_type {
        CLOSE_BRACKET,
        COMMA,
        EQUAL,
        ESCAPE,
        NEGATIVE,
        OPEN_BRACKET,
        PERIOD,
        QUOTE,
        UNKNOWN,
};

struct parser_context {
        size_t line_num;
        size_t col_num;
        const struct sroc_value *current_value;
        const struct sroc_table *current_table;
};

enum token_type char_to_token(char input);

struct parser_context *init_parser(void);
void destroy_parser_context(struct parser_context *context);

int64_t find_first_nonspace(const char *string);
int64_t find_last_nonspace(const char *string);
int64_t strip_surrounding_space(const char *string, char **dest);
