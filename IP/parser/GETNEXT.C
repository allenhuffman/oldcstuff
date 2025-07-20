#include <stdio.h>
#include <stdlib.h>

int
get_next( FILE *infp, char *token )
{
    int i;
    
    i = fscanf( infp, "%s", token );

    if ( i != 1 ) {
exit(0);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
