#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
filelist( FILE *infp, char *token )
{
    do {
        file( infp, token );

        get_next( infp, token );

        if ( strstr( token, "]" ) != 0 ) {
/*            printf( "] end of file list.\n" ); */
            return FALSE;
        }

    } while( 1 );

    return FALSE;
}

int
file( FILE *infp, char *token )
{

    printf( "put %s ", token );

    get_next( infp, token );

    printf( "%s\n", token );

    return FALSE;
}
