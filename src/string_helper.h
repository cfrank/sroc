// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include <stdbool.h>

int64_t string_get_delimiter(const char *string, char **dest, char delimiter,
                             bool consume);
int64_t string_get_line(const char *string, char **dest);
int64_t string_find_first_nonspace(const char *string);
int64_t string_find_last_nonspace(const char *string);
int64_t string_splice(const char *string, char **dest, int64_t start,
                      int64_t end);
int64_t string_strip_surrounding_space(const char *string, char **dest);
