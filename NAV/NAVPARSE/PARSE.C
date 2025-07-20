#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
itemlist( FILE *infp, char *token )
{
    do {
        item( infp, token );

        get_next( infp, token );

        if ( strstr( token, "]" ) != 0 ) {
/*            printf( "] end of item list.\n" );*/
            return FALSE;
        }

    } while( 1 );

    return FALSE;
}

int
item( FILE *infp, char *token )
{
    printf( "ITEM: %s\n", token );

    get_next( infp, token );

    if ( strstr( token, "[" ) != 0 ) {

        get_next( infp, token );
        
        itemlist( infp, token );

        return FALSE;
    }

    while( item( infp, token ) );

    return FALSE;
}
