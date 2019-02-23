// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include <stdint.h>

ssize_t read_until_delimiter(char *string, char delimiter, char **buffer,
                             size_t *size, bool consume);
