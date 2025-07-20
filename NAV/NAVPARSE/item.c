#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

extern int level;

int
itemlist( FILE *infp, char *token )
{
    do {
        while( item( infp, token ) );

        get_next( infp, token );

        if ( strstr( token, "]" ) != 0 ) {
            level--;
            return TRUE;
        }

    } while( 1 );

    return FALSE;
}

int
item( FILE *infp, char *token )
{
    int i;

    get_next( infp, token );

    if ( strstr( "[", token)!=0 ) {
        level++;
        itemlist( infp, token );
        return TRUE;
    }
    
    for ( i=0 ; i<level*3 ; i++ ) printf( " " );
    printf( "[+]---%s\n", token );

    return FALSE;
}
