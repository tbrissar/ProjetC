#include <stdio.h>
#include <stdlib.h>

#include "myassert.h"

void myassert_func(bool condition, const char *fileName, const char *functionName, int line)
{
    if (! (condition))
    {
        fprintf(stderr, "ERROR %s %s l.%d\n", fileName, functionName, line);
        perror(NULL);
        //exit(EXIT_FAILURE);
    }

}
