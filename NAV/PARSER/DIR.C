#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
dirlist( FILE *infp, char *token )
{
    do {
        dir( infp, token );

        get_next( infp, token );

        if ( strstr( token, "]" ) != 0 ) {
/*            printf( "] end of dir list.\n" ); */
            return FALSE;
        }

    } while( 1 );

    return FALSE;
}

int
dir( FILE *infp, char *token )
{

    printf( "cd %s\n", token );

    get_next( infp, token );
    
    mode( infp, token );

    return FALSE;
}
