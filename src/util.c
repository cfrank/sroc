// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

ssize_t read_until_delimiter(char *string, char delimiter, char **buffer,
                             size_t *size, bool consume)
{
        char ch;
        char *buffer_pos;

        if (buffer == NULL || size == NULL || string == NULL) {
                errno = EINVAL;
                return -1;
        }

        if (buffer == NULL || *size == 0) {
                // Empty buffer supplied
                *size = 128;
                *buffer = malloc(*size);

                if (buffer == NULL) {
                        errno = ENOMEM;
                        return -1;
                }
        }

        buffer_pos = *buffer;

        for (;;) {
                ch = *string;

                if (ch == EOF) {
                        break;
                }

                if ((buffer_pos + 1) == (*buffer + *size)) {
                        // No more room in the buffer
                        size_t new_size = *size * 2;
                        char *realloc_buffer = realloc(*buffer, new_size);

                        if (realloc_buffer == NULL) {
                                errno = ENOMEM;
                                return -1;
                        }

                        buffer_pos = realloc_buffer + (buffer_pos - *buffer);
                        *buffer = realloc_buffer;
                        *size = new_size;
                }

                *buffer_pos++ = ch;

                if (ch == delimiter) {
                        if (!consume) {
                                // If not consuming delimiter roll back buffer
                                buffer_pos--;
                        }

                        break;
                }
        }

        if (ch == EOF && buffer_pos == *buffer) {
                // Nothing was read
                return -1;
        }

        return buffer_pos - *buffer;
}
