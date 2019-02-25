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
        struct sroc_root *root;
        size_t line_num;
        size_t col_num;
        struct sroc_value *current_value;
        struct sroc_table *current_table;
};

struct parser_context *init_parser(struct sroc_root *root);
enum token_type char_to_token(char input);

