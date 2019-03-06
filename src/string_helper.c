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
 * Find the first index of a supplied elimiter character
 *
 * reads the supplied string until a supplied terminating char
 */
int64_t string_find_delimiter(const char *string, char delimiter,
                              char terminator)
{
        if (string == NULL) {
                return -1;
        }

        char ch;
        int64_t index = 0;

        while ((ch = string[index]) != terminator && ch != '\0') {
                ++index;
        }

        if (ch == '\0') {
                return -1;
        }

        return index;
}

/*
 * Find the last index of a supplied delimiter character
 *
 * reads the supplied string until a supplied terminating char
 */
int64_t string_find_last_delimiter(const char *string, char delimiter,
                                   char terminator)
{
        size_t string_length = strlen(string);

        if (string == NULL || string_length < 1) {
                return -1;
        }

        char ch;
        int64_t index = (int64_t)string_length - 1;

        while ((ch = string[index]) != terminator && index > 0) {
                --index;
        }

        return index;
}

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

        int64_t result_length = delimiter_location - string;

        // Make sure a valid cast to size_t can be accomplished
        assert(result_length >= 0);

        char *buffer = malloc((size_t)result_length + 1);

        if (buffer == NULL) {
                errno = ENOMEM;

                return -1;
        }

        *dest = buffer;

        memcpy(*dest, string, (size_t)result_length);

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
        if (string == NULL) {
                return -1;
        }

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
        if (string == NULL) {
                return -1;
        }

        int64_t index = (int64_t)strlen(string) - 1;

        while (index >= 0 && isspace(string[index])) {
                --index;
        }

        // If nothing is found will have reached -1
        return index;
}

/**
 * Extracts a porition of a string into dest specified by the start and end
 * indexes. If nothing is found or there is an error -1 is returned.
 *
 * If a substring is found the substring is placed into dest along with an
 * added null terminator. dest must be freed after use. The strlen of the
 * substring is returned to the caller.
 */
int64_t string_splice(const char *string, char **dest, int64_t start,
                      int64_t end)
{
        if (string == NULL || end < start || end < 0 || start < 0
            || end > (int64_t)strlen(string)) {
                return -1;
        }

        int64_t spliced_size = end - start;

        assert(spliced_size >= 0);

        *dest = malloc((size_t)spliced_size + 1);

        if (*dest == NULL) {
                return -1;
        }

        memcpy(*dest, string + start, (size_t)spliced_size);

        (*dest)[spliced_size] = '\0';

        return spliced_size;
}

/**
 * Strips the whitespace surrounding a string and places that new string into
 * the destination with an added null terminator.
 *
 * The strlen of the new string is returned.
 */
int64_t string_strip_surrounding_space(const char *string, char **dest)
{
        if (string == NULL || dest == NULL) {
                return -1;
        }

        int64_t start_index = string_find_first_nonspace(string);
        int64_t end_index = string_find_last_nonspace(string);

        if (start_index < 0 || end_index < 0) {
                return -1;
        }

        return string_splice(string, dest, start_index, end_index + 1);
}
