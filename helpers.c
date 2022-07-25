/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

void die(const char *text)
{
    fprintf(stderr, "%s", text);
    exit(1);
}
