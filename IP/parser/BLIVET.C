#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
blivet( FILE *infp, char *buffer )
{
    printf( "o %s\n", buffer );

    get_next( infp, buffer );
    
    if ( strstr( "[", buffer ) != 0 ) {
/*        printf( "[ beginning of userlist.\n" ); */

        get_next( infp, buffer );
        
        userlist( infp, buffer );

        return FALSE;
    }

    while( user( infp, buffer ) );
    
    return TRUE;
}
