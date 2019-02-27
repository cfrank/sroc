// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "string_helper.h"

/*
 * Gets all characters up to a supplied delimiter character.
 *
 * It is assumed that dest is a uninitialized char pointer, which will be
 * initialized by this function and freed by the caller
 *
 * If a valid string is found the strlen of the resulting string is returned
 * and the string is sent into dest with an appended null terminator
 */
int64_t string_get_delimiter(const char *string, char delimiter, char **dest)
{
        const char *delimiter_location = strchr(string, delimiter);

        if (delimiter_location == NULL) {
                return -1;
        }

        int64_t result_length = (delimiter_location - string);

        assert(result_length >= 0);

        char *buffer = malloc((size_t)result_length + 1);

        if (buffer == NULL) {
                errno = ENOMEM;

                return -1;
        }

        *dest = buffer;

        memcpy(*dest, string, result_length);

        (*dest)[result_length] = '\0';

        return result_length;
}

/*
 * Gets all characters up to the first new line character.
 *
 * It is assumed that dest is a uninitialized char pointer, which will be
 * initialized by this function and freed by the caller
 *
 * If a valid string is found the strlen of the resulting string is returned
 * and the string is sent into dest with an appended null terminator
 */
int64_t string_get_line(const char *string, char **dest)
{
        return string_get_delimiter(string, '\n', dest);
}

/**
 * Finds the first nonspace character in a string. If nothing is found -1 is
 * returned.
 *
 * If a nonspace character is found the index of that character is returned
 */
int64_t string_find_first_nonspace(const char *string)
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
int64_t string_find_last_nonspace(const char *string)
{
        int64_t index = (int64_t)strlen(string) - 1;

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
int64_t string_strip_surrounding_space(const char *string, char **dest)
{
        int64_t start_index = string_find_first_nonspace(string);
        int64_t end_index = string_find_last_nonspace(string);

        if (start_index < 0 || end_index < 0) {
                return -1;
        }

        int64_t stripped_size = (end_index - start_index) + 1;

        assert(stripped_size >= 0);

        *dest = malloc((size_t)stripped_size + 1);

        if (*dest == NULL) {
                return -1;
        }

        memcpy(*dest, string + start_index, stripped_size);

        (*dest)[stripped_size] = '\0';

        return stripped_size;
}

