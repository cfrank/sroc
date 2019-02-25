#include <stdio.h>
#include <stdlib.h>

#include <sroc.h>

static void handle_file(FILE *file)
{
        struct sroc_root *root = sroc_parse_file(file);

        sroc_destroy_root(root);
}

int main(int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "Usage: cmd <file>\n");

                return EXIT_FAILURE;
        }

        FILE *conf_file = fopen(argv[1], "r");

        if (conf_file == NULL) {
                fprintf(stderr, "Failed to open file for reading\n");

                return EXIT_FAILURE;
        }

        handle_file(conf_file);

        fclose(conf_file);

        return EXIT_SUCCESS;
}
