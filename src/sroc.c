// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <stdio.h>
#include <stdlib.h>

#include "sroc.h"

struct sroc_root *sroc_init(FILE *file)
{
        // TODO: Handle opening file

        return NULL;
}

struct sroc_table *sroc_create_table(char *key)
{
        struct sroc_table *table = malloc(sizeof(struct sroc_table));

        if (table == NULL) {
                return NULL;
        }

        table->key = key;
        table->size = 0;
        table->items = NULL;

        return table;
}

void sroc_destroy_root(struct sroc_root *root)
{
        for (size_t i = 0; i < root->items_length; ++i) {
                sroc_destroy_item(root->items[i]);
        }

        for (size_t i = 0; i < root->sections_length; ++i) {
                sroc_destroy_table(root->sections[i]);
        }

        free(root);
}

void sroc_destroy_array(struct sroc_array *array)
{
        for (size_t i = 0; i < array->length; ++i) {
                sroc_destroy_value(array->items[i]);
        }

        free(array);
}

void sroc_destroy_item(struct sroc_item *item)
{
        free(item->key);

        sroc_destroy_value(item->value);
}

void sroc_destroy_value(struct sroc_value *value)
{
        if (value->type == SROC_ARRAY) {
                sroc_destroy_array(value->array);
        } else if (value->type == SROC_STRING) {
                free(value->string);
        }

        free(value);
}

void sroc_destroy_table(struct sroc_table *table)
{
        free(table->key);

        for (size_t i = 0; i < table->size; ++i) {
                sroc_destroy_item(table->items[i]);
        }

        free(table);
}

