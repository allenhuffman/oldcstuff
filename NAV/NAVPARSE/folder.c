#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
folder( FILE *infp, char *buffer )
{
    get_next( infp, buffer );
    
    if ( strstr( "[", buffer ) != 0 ) {
        printf( "[ beginning of sub-FOLDER.\n" );

        folder( infp, buffer );

        return FALSE;
    }

    while( itemlist( infp, buffer ) );
    
    return TRUE;
}
