#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
userlist( FILE *infp, char *token )
{
    do {
        user( infp, token );

        get_next( infp, token );

        if ( strstr( token, "]" ) != 0 ) {
/*            printf( "] end of user list.\n" );*/
            return FALSE;
        }

    } while( 1 );

    return FALSE;
}

int
user( FILE *infp, char *token )
{
    printf( "%s\n", token );

    get_next( infp, token );

    printf( "%s\n", token );

    get_next( infp, token );

    if ( strstr( token, "[" ) != 0 ) {

        get_next( infp, token );
        
        dirlist( infp, token );

        return FALSE;
    }

    while( dir( infp, token ) );

    return FALSE;
}
