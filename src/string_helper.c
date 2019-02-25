// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "string_helper.h"

int string_get_delimiter(const char *string, char delimiter, char **dest)
{
        const char *delimiter_location = strchr(string, delimiter);

        if (delimiter_location == NULL) {
                return -1;
        }

        size_t result_length = (delimiter_location - string);

        char *buffer = malloc(result_length + 1);

        if (buffer == NULL) {
                errno = ENOMEM;

                return -1;
        }

        *dest = buffer;

        memcpy(*dest, string, result_length);

        (*dest)[result_length] = '\0';

        return result_length;
}

int string_get_line(const char *string, char **dest)
{
        return string_get_delimiter(string, '\n', dest);
}
