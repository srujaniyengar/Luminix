#include <stdio.h>
#include "luminix.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s <source_image> <target_image> <output_image>\n", argv[0]);
        return 1;
    }
    int status = luminix(argv[1], argv[2], argv[3]);
    if (status != 0) {
        printf("Luminix failed with error code %d\n", status);
        return status;
    }
    printf("Luminix completed successfully.\n");
    return 0;
}