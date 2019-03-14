// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include "sroc.h"

enum token_type {
        ALPHA_CHAR,
        CLOSE_BRACKET,
        COMMENT_START,
        COMMA,
        EQUAL,
        ESCAPE,
        NEGATIVE,
        NUMERIC_CHAR,
        OPEN_BRACKET,
        PERIOD,
        QUOTE,
        UNKNOWN,
};

struct parser_context {
        const char *buffer;
        size_t pos;
        size_t line_num;
        size_t col_num;
        const struct sroc_value *current_value;
        const struct sroc_table *current_table;
};

enum token_type char_to_token(char input);
int parse_line(struct parser_context *context, char ch);

struct parser_context *init_parser(void);
void increment_parser_context(struct parser_context *context);

struct sroc_table *get_section(struct parser_context *context);
bool is_valid_declaration(struct parser_context *context);
void destroy_parser_context(struct parser_context *context);

