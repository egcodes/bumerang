#include "bumerang_messages.h"

#include <stdio.h>
#include <stdlib.h>

void bumerang_info(char *message)
{
    printf("* %s...\n", message);
}

void bumerang_error(char *message)
{
    printf("%s...\n", message);
    exit(EXIT_FAILURE);
}
